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
  if (parent.isValid())
    return 0;

  return m_booksCount;
}

bool BookRestModel::canFetchMore(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return false;
  }

  return m_booksCount < m_pagination->perPage() &&
         m_booksCount < m_pagination->totalCount();
}

void BookRestModel::fetchMore(const QModelIndex &parent) {
  if (parent.isValid()) {
    return;
  }
  static const int k_fetchSize = 10;

  int numItemsLeft = int(m_bookCards.size() - m_booksCount);
  int numItemsFetch = qMin(k_fetchSize, numItemsLeft);

  beginInsertRows({}, m_booksCount, m_booksCount + numItemsFetch - 1);

  for (int row = m_booksCount; row < m_booksCount + numItemsFetch; ++row) {
    const BookCard &bookCard = m_bookCards[row];
    processImageUrl(row, bookCard.coverUrl());
  }
  m_booksCount += numItemsFetch;

  endInsertRows();
}

Qt::ItemFlags BookRestModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

bool BookRestModel::setData(const QModelIndex &index, const QVariant &value,
                            int role) {
  if (!index.isValid()) {
    return false;
  }

  int row = index.row();
  if (row < 0 || row >= m_bookCards.size() || data(index, role) == value) {
    return false;
  }

  BookCard &bookCard = m_bookCards[row];

  switch (role) {
    case ButtonStateRole:
      bookCard.setButtonState(value.value<QStyle::State>());
      break;
    case IdRole:
      bookCard.setBookId(value.toUInt());
      break;
    case HoverRatingRole:
      bookCard.setHoverRating(value.toInt());
      break;
    case TitleRole:
      bookCard.setTitle(value.toString());
      break;
    case CoverRole:
      bookCard.setCover(value.value<QPixmap>());
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
  if (!index.isValid()) {
    return false;
  }

  int row = index.row();
  if (row < 0 || row >= m_bookCards.size()) {
    return false;
  }

  const BookCard &bookCard = m_bookCards.at(row);

  switch (role) {
    case Qt::SizeHintRole:
      return bookCard.sizeHint();
    case Qt::DisplayRole:
      return QVariant::fromValue(m_bookCards.at(row));
    case IdRole:
      return bookCard.bookId();
    case HoverRatingRole:
      return bookCard.hoverRating();
    case ButtonStateRole:
      return QVariant::fromValue(bookCard.buttonState());
    case TitleRole:
      return bookCard.title();
    case CoverRole:
      return bookCard.cover();
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
  for (auto &work : m_imageWork) {
    work.reply->abort();
  }
  m_imageWork.clear();
  m_booksCount = 0;

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

  QUrl k_testUrl("https://upload.wikimedia.org/wikipedia/en/thumb/0/0b/"
                 "Your_Name_poster.png/220px-Your_Name_poster.png");
  auto [future, reply] = imageLoader.load(k_testUrl);
  m_imageWork.insert(row, {reply, busyIndicator});

  busyIndicator->start();

  m_bookCards[row].setBusyIndicator(busyIndicator);

  connect(busyIndicator.get(), &QMovie::frameChanged, this, [this](int) {
    for (int row : m_imageWork.keys()) {
      QModelIndex idx = index(row);
      emit dataChanged(idx, idx, {CoverRole});
    }
  });

  future
    .then(this, [this, row](const QPixmap &pixmap) {
    setData(index(row), pixmap, CoverRole);
    m_imageWork.remove(row);
  }).onFailed([this, row](const NetworkError & /*err*/) {
    QPixmap defaultCover(":/images/DefaultBookCover");
    setData(index(row), defaultCover, CoverRole);
    m_imageWork.remove(row);
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
  return m_bookCards.at(row);
}
