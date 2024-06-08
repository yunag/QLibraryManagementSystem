#include <QJsonArray>
#include <QMovie>

#include "bookrestmodel.h"
#include "modelhelper.h"

#include "common/algorithm.h"
#include "common/json.h"

#include "resourcemanager.h"

BookRestModel::BookRestModel(QObject *parent)
    : AbstractRestModel(parent), m_shouldFetchImages(false), m_booksCount(0) {
  setRoute("/api/books");
  setRestManager(ResourceManager::networkManager());
}

int BookRestModel::rowCount(const QModelIndex &parent) const {
  CHECK_ROWCOUNT(parent);

  return m_booksCount;
}

int BookRestModel::columnCount(const QModelIndex &parent) const {
  CHECK_COLUMNCOUNT(parent);

  return LastHeader + 1;
}

bool BookRestModel::canFetchMore(const QModelIndex &parent) const {
  CHECK_CANFETCHMORE(parent);

  return m_booksCount < m_bookCards.size();
}

void BookRestModel::fetchMore(const QModelIndex &parent) {
  CHECK_FETCHMORE(parent);

  static const int k_fetchSize = 10;

  int numItemsLeft = static_cast<int>(m_bookCards.size()) - m_booksCount;
  int numItemsFetch = qMin(k_fetchSize, numItemsLeft);

  beginInsertRows({}, m_booksCount, m_booksCount + numItemsFetch - 1);

  m_booksCount += numItemsFetch;

  endInsertRows();

  if (!m_shouldFetchImages) {
    return;
  }

  for (int row = m_booksCount - numItemsFetch; row < m_booksCount; ++row) {
    BookCard &bookCard = m_bookCards[row];

    auto busyIndicator = ResourceManager::busyIndicator();
    processImageUrl(row, bookCard.coverUrl(), busyIndicator, CoverRole);
    bookCard.setBusyIndicator(busyIndicator);
  }
}

bool BookRestModel::setData(const QModelIndex &index, const QVariant &value,
                            int role) {
  CHECK_DATA(index);

  if (data(index, role) == value) {
    return false;
  }

  BookCard &bookCard = m_bookCards[index.row()];

  switch (role) {
    case ButtonStateRole:
      bookCard.setButtonState(value.value<QStyle::State>());
      break;
    case HoverRatingRole:
      emit dataChanged(m_hoverRating.index, m_hoverRating.index, {role});
      m_hoverRating = {index, value.toInt()};
      break;
    case CoverRole:
      bookCard.setCover(value.value<QPixmap>());
      break;
    case RatingRole:
      bookCard.setRating(value.toInt());
      break;
    default:
      AbstractRestModel::setData(index, value, role);
      break;
  }

  emit dataChanged(index, index, {role});
  return true;
}

QVariant BookRestModel::data(const QModelIndex &index, int role) const {
  CHECK_DATA(index);

  const BookCard &bookCard = m_bookCards.at(index.row());

  switch (role) {
    case Qt::DisplayRole:
      return dataForColumn(index);
    case ObjectRole:
      return QVariant::fromValue(bookCard);
    case IdRole:
      return bookCard.bookId();
    case HoverRatingRole:
      if (index == m_hoverRating.index) {
        return m_hoverRating.rating;
      }
      return -1;
    case ButtonStateRole:
      return QVariant::fromValue(bookCard.buttonState());
    case TitleRole:
      return bookCard.title();
    case CoverRole:
      return bookCard.cover();
    case CoverUrlRole:
      return bookCard.coverUrl();
    case AuthorsRole:
      return bookCard.authors();
    case CategoriesRole:
      return bookCard.categories();
    case RatingRole:
      return bookCard.rating();
    default:
      return {};
  }
}

void BookRestModel::onBookDetails(const BookDetails &details) {
  auto authors = algorithm::transform<QStringList>(details.authors,
                                                   [](const Author &author) {
    return author.firstName + " " + author.lastName;
  });

  auto categories = algorithm::transform<QStringList>(
    details.categories, [](const Category &category) { return category.name; });

  BookCard bookCard;
  bookCard.setTitle(details.title);
  bookCard.setBookId(details.id);
  bookCard.setAuthors(authors);
  bookCard.setCategories(categories);
  bookCard.setRating(qRound(details.rating));
  bookCard.setCoverUrl(details.coverUrl);

  m_bookCards.push_back(bookCard);
}

QUrlQuery BookRestModel::includeFieldsQuery() {
  QUrlQuery query;
  query.addQueryItem("includeauthors", {});
  query.addQueryItem("includecategories", {});
  return query;
}

void BookRestModel::handleRequestData(const QByteArray &data) {
  QJsonArray books = json::byteArrayToJson(data)->array();
  auto booksDetails =
    algorithm::transform<QList<BookDetails>>(books, [](auto bookJson) {
    return BookDetails::fromJson(bookJson.toObject());
  });

  int numItemsToInsert = static_cast<int>(booksDetails.size());
  if (!numItemsToInsert) {
    return;
  }

  for (const auto &bookDetails : booksDetails) {
    onBookDetails(bookDetails);
  }

  fetchMore({});
}

bool BookRestModel::removeRows(int row, int count, const QModelIndex &parent) {
  CHECK_REMOVEROWS(row, count, parent);

  beginRemoveRows({}, row, row + count - 1);

  m_bookCards.remove(row, count);
  m_booksCount -= count;

  endRemoveRows();

  return true;
}

const BookCard &BookRestModel::get(int row) const {
  Q_ASSERT(row >= 0 && row < rowCount());

  return m_bookCards.at(row);
}

void BookRestModel::shouldFetchImages(bool shouldFetchImages) {
  m_shouldFetchImages = shouldFetchImages;

  if (!m_shouldFetchImages) {
    return;
  }

  for (int row = 0; row < rowCount(); ++row) {
    BookCard &card = m_bookCards[row];

    if (card.cover().isNull()) {
      auto busyIndicator = ResourceManager::busyIndicator();

      processImageUrl(row, card.coverUrl(), busyIndicator, CoverRole);
      card.setBusyIndicator(ResourceManager::busyIndicator());
    }
  }
}

QVariant BookRestModel::dataForColumn(const QModelIndex &index) {
  switch (index.column()) {
    case IdHeader:
      return index.data(IdRole);
    case TitleHeader:
      return index.data(TitleRole);
    case AuthorsHeader:
      return index.data(AuthorsRole).toStringList().join(", ");
    case CategoriesHeader:
      return index.data(CategoriesRole).toStringList().join(", ");
    case RatingHeader:
      return index.data(BookRestModel::RatingRole);
    default:
      return {};
  }
};

QVariant BookRestModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  CHECK_HEADERDATA(section, orientation);

  if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
    return QAbstractListModel::headerData(section, orientation, role);
  }

  switch (section) {
    case IdHeader:
      return tr("Id");
    case TitleHeader:
      return tr("Title");
    case AuthorsHeader:
      return tr("Authors");
    case CategoriesHeader:
      return tr("Categories");
    case RatingHeader:
      return tr("Rating");
    default:
      return "";
  }
}

void BookRestModel::reset() {
  beginResetModel();
  abortReplies();

  m_bookCards.clear();
  m_booksCount = 0;

  endResetModel();
}
