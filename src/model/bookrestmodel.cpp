#include <QJsonArray>
#include <QMovie>

#include "bookrestmodel.h"

#include "common/algorithm.h"
#include "common/json.h"

#include "network/imageloader.h"

#include "libraryapplication.h"

BookRestModel::BookRestModel(QObject *parent)
    : QAbstractListModel(parent), m_booksCount(0) {}

int BookRestModel::rowCount(const QModelIndex &parent) const {
  if (!checkIndex(parent)) {
    return 0;
  }

  return m_booksCount;
}

int BookRestModel::columnCount(const QModelIndex &parent) const {
  if (!checkIndex(parent)) {
    return 0;
  }

  return Last + 1;
}

bool BookRestModel::canFetchMore(const QModelIndex &parent) const {
  if (!checkIndex(parent)) {
    return false;
  }

  return m_booksCount < m_pagination->perPage() &&
         m_booksCount < m_pagination->totalCount();
}

void BookRestModel::fetchMore(const QModelIndex &parent) {
  if (!checkIndex(parent)) {
    return;
  }

  static const int k_fetchSize = 10;

  int numItemsLeft = int(m_bookCards.size() - m_booksCount);
  int numItemsFetch = qMin(k_fetchSize, numItemsLeft);

  beginInsertRows({}, m_booksCount, m_booksCount + numItemsFetch - 1);

  for (int row = m_booksCount; row < m_booksCount + numItemsFetch; ++row) {
    const BookCard &bookCard = m_bookCards[row];
    if (m_shouldFetchImages) {
      processImageUrl(row, bookCard.coverUrl());
    }
  }
  m_booksCount += numItemsFetch;

  endInsertRows();
}

bool BookRestModel::setData(const QModelIndex &index, const QVariant &value,
                            int role) {
  if (!checkIndex(index, CheckIndexOption::IndexIsValid |
                           CheckIndexOption::ParentIsInvalid)) {
    return false;
  }

  if (data(index, role) == value) {
    return false;
  }

  BookCard &bookCard = m_bookCards[index.row()];

  switch (role) {
    case ButtonStateRole:
      bookCard.setButtonState(value.value<QStyle::State>());
      break;
    case IdRole:
      bookCard.setBookId(value.toUInt());
      break;
    case HoverRatingRole:
      emit dataChanged(m_hoverRating.index, m_hoverRating.index, {role});
      m_hoverRating = {index, value.toInt()};
      break;
    case TitleRole:
      bookCard.setTitle(value.toString());
      break;
    case CoverRole:
      bookCard.setCover(value.value<QPixmap>());
      break;
    case CoverUrlRole:
      bookCard.setCoverUrl(value.toUrl());
      break;
    case AuthorsRole:
      bookCard.setAuthors(value.toStringList());
      break;
    case CategoriesRole:
      bookCard.setCategories(value.toStringList());
      break;
    case RatingRole:
      bookCard.setRating(value.toInt());
      break;
    default:
      QAbstractListModel::setData(index, value, role);
      break;
  }

  emit dataChanged(index, index, {role});
  return true;
}

QVariant BookRestModel::data(const QModelIndex &index, int role) const {
  if (!checkIndex(index, CheckIndexOption::IndexIsValid)) {
    return {};
  }

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
  return {};
}

void BookRestModel::onBookData(const BookData &bookData) {
  auto authors = algorithm::transform<QStringList>(bookData.authors,
                                                   [](const Author &author) {
    return author.firstName + " " + author.lastName;
  });

  auto categories = algorithm::transform<QStringList>(
    bookData.categories,
    [](const Category &category) { return category.name; });

  BookCard bookCard;
  bookCard.setTitle(bookData.title);
  bookCard.setBookId(bookData.id);
  bookCard.setAuthors(authors);
  bookCard.setCategories(categories);
  bookCard.setRating(qRound(bookData.rating));
  bookCard.setCoverUrl(bookData.coverUrl);

  m_bookCards.push_back(bookCard);
}

void BookRestModel::updatePagination(ReplyPointer reply) {
  QByteArray currentPage = m_pagination->currentPageHeader().toUtf8();
  QByteArray totalCount = m_pagination->totalCountHeader().toUtf8();
  QByteArray pageCount = m_pagination->pageCountHeader().toUtf8();

  m_pagination->setCurrentPage(reply->rawHeader(currentPage).toInt());
  m_pagination->setTotalCount(reply->rawHeader(totalCount).toInt());
  m_pagination->setPageCount(reply->rawHeader(pageCount).toInt());
}

void BookRestModel::reset() {
  beginResetModel();

  m_bookCards.clear();
  m_booksCount = 0;
  for (auto &work : m_imageWork) {
    work.reply->abort();
  }
  m_imageWork.clear();

  endResetModel();
}

QVariantMap BookRestModel::filters() const {
  return m_filters;
}

void BookRestModel::setFilters(const QVariantMap &newFilters) {
  if (m_filters == newFilters)
    return;

  m_filters = newFilters;
  emit filtersChanged();
}

Pagination *BookRestModel::pagination() const {
  return m_pagination;
}

void BookRestModel::setPagination(Pagination *newPagination) {
  if (m_pagination == newPagination)
    return;

  m_pagination = newPagination;

  emit paginationChanged();
}

QString BookRestModel::orderBy() const {
  return m_orderBy;
}

void BookRestModel::setOrderBy(const QString &newOrderBy) {
  if (m_orderBy == newOrderBy)
    return;

  m_orderBy = newOrderBy;
  emit orderByChanged();
}

void BookRestModel::processImageUrl(int row, const QUrl &url) {
  ImageLoader imageLoader(App->network());

  auto busyIndicator = App->busyIndicator();

  auto [future, reply] = imageLoader.load(url);

  m_imageWork[row] = {reply, busyIndicator};
  m_bookCards[row].setBusyIndicator(busyIndicator);

  busyIndicator->start();

  connect(busyIndicator.get(), &QMovie::frameChanged, this, [this](int) {
    for (int row : m_imageWork.keys()) {
      QModelIndex idx = index(row);
      emit dataChanged(idx, idx, {CoverRole});
    }
  });

  auto handleImage = [this, row](const QPixmap &pixmap) {
    if (m_imageWork.contains(row)) {
      setData(index(row), pixmap, CoverRole);
      m_imageWork.remove(row);
    }
  };

  future
    .then(this, [handleImage](const QPixmap &pixmap) {
    handleImage(pixmap);
  }).onFailed([handleImage](const NetworkError &err) {
    if (err.type() != QNetworkReply::OperationCanceledError) {
      QPixmap defaultCover(":/images/DefaultBookCover");
      handleImage(defaultCover);
    }
  });
}

void BookRestModel::reload() {
  QUrlQuery query;

  for (const auto &[filter, value] : m_filters.asKeyValueRange()) {
    query.addQueryItem(filter, value.toString());
  }

  query.addQueryItem("orderby", m_orderBy);
  query.addQueryItem("page", QString::number(m_pagination->currentPage()));
  query.addQueryItem("perpage", QString::number(m_pagination->perPage()));
  query.addQueryItem("includeauthors", {});
  query.addQueryItem("includecategories", {});

  RestApiManager *networkManager = App->network();

  auto [future, reply] = networkManager->get("/api/books", query);

  future
    .then([this, replyPtr = reply](const QByteArray &data) {
    updatePagination(replyPtr);

    QJsonArray books = json::byteArrayToJson(data)->array();
    auto booksData =
      algorithm::transform<QList<BookData>>(books, [](auto bookJson) {
      return BookData::fromJson(bookJson.toObject());
    });

    int numItemsToInsert = int(booksData.size());
    if (!numItemsToInsert) {
      return;
    }

    for (const auto &bookData : booksData) {
      onBookData(bookData);
    }

    fetchMore({});

    emit reloadFinished();
  }).onFailed(this, []() {

  });
}

bool BookRestModel::removeRows(int row, int count, const QModelIndex &parent) {
  if (parent.isValid()) {
    return false;
  }

  if (!count || row < 0 || row >= rowCount()) {
    return false;
  }

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

  for (int i = 0; i < rowCount(); ++i) {
    const BookCard &card = m_bookCards[i];

    if (card.cover().isNull()) {
      processImageUrl(i, card.coverUrl());
    }
  }
}

QVariant BookRestModel::dataForColumn(const QModelIndex &index) const {
  switch (index.column()) {
    case Id:
      return index.data(IdRole);
    case Title:
      return index.data(TitleRole);
    case Authors:
      return index.data(AuthorsRole).toStringList().join(", ");
    case Categories:
      return index.data(BookRestModel::CategoriesRole)
        .toStringList()
        .join(", ");
    case Rating:
      return index.data(BookRestModel::RatingRole);
    default:
      return {};
  }
};

QVariant BookRestModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const {
  if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
    return QAbstractListModel::headerData(section, orientation, role);
  }

  switch (section) {
    case Id:
      return tr("Id");
    case Title:
      return tr("Title");
    case Authors:
      return tr("Authors");
    case Categories:
      return tr("Categories");
    case Rating:
      return tr("Rating");
    default:
      return "";
  }
}
