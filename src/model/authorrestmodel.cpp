#include <QJsonArray>

#include "common/algorithm.h"
#include "common/json.h"

#include "authorrestmodel.h"
#include "modelhelper.h"
#include "resourcemanager.h"

AuthorRestModel::AuthorRestModel(QObject *parent)
    : AbstractRestModel(parent), m_authorsCount(0) {
  setRoute("/api/authors");
  setRestManager(ResourceManager::networkManager());
}

void AuthorRestModel::handleRequestData(const QByteArray &data) {
  QJsonArray authors = json::byteArrayToJson(data)->array();
  m_authors = algorithm::transform<QList<Author>>(authors, [](auto authorJson) {
    return Author::fromJson(authorJson.toObject());
  });

  fetchMore({});
};

int AuthorRestModel::columnCount(const QModelIndex &parent) const {
  CHECK_COLUMNCOUNT(parent);
  return LastHeader;
}

int AuthorRestModel::rowCount(const QModelIndex &parent) const {
  CHECK_ROWCOUNT(parent);
  return static_cast<int>(m_authors.size());
}

bool AuthorRestModel::removeRows(int row, int count,
                                 const QModelIndex &parent) {
  CHECK_REMOVEROWS(row, count, parent);

  beginRemoveRows({}, row, row + count - 1);
  m_authors.remove(row, count);
  endRemoveRows();

  return true;
}

bool AuthorRestModel::canFetchMore(const QModelIndex &parent) const {
  CHECK_CANFETCHMORE(parent);

  return m_authorsCount < m_pagination->perPage() &&
         m_authorsCount < m_pagination->totalCount();
}

void AuthorRestModel::fetchMore(const QModelIndex &parent) {
  CHECK_FETCHMORE(parent);

  static const int k_fetchSize = 10;

  int numItemsLeft = static_cast<int>(m_authors.size()) - m_authorsCount;
  int numItemsFetch = qMin(k_fetchSize, numItemsLeft);

  beginInsertRows({}, m_authorsCount, m_authorsCount + numItemsFetch - 1);

  m_authorsCount += numItemsFetch;

  endInsertRows();

  if (!m_shouldFetchImages) {
    return;
  }

  for (int row = m_authorsCount - numItemsFetch; row < m_authorsCount; ++row) {
    const auto &author = m_authors.at(row);

    auto busyIndicator = ResourceManager::busyIndicator();
    processImageUrl(row, author.imageUrl, busyIndicator, ImageRole);
  }
}

void AuthorRestModel::reset() {
  beginResetModel();
  abortReplies();

  m_authors.clear();
  m_authorsCount = 0;
  endResetModel();
}

void AuthorRestModel::shouldFetchImages(bool shouldFetchImages) {
  m_shouldFetchImages = shouldFetchImages;

  if (!m_shouldFetchImages) {
    return;
  }

  for (int i = 0; i < rowCount(); ++i) {
    Author &author = m_authors[i];

    if (data(index(i), ImageRole).isNull()) {
      auto busyIndicator = ResourceManager::busyIndicator();

      processImageUrl(i, author.imageUrl, busyIndicator, ImageRole);
    }
  }
}

QVariant AuthorRestModel::data(const QModelIndex &index, int role) const {
  CHECK_DATA(index);

  const Author &author = m_authors.at(index.row());

  switch (role) {
    case Qt::DisplayRole:
      return dataForColumn(index);
    case IdRole:
      return author.id;
    case ImageUrlRole:
      return author.imageUrl;
    case FirstNameRole:
      return author.firstName;
    case LastNameRole:
      return author.lastName;

    default:
      return {};
  }
}

QVariant AuthorRestModel::dataForColumn(const QModelIndex &index) {
  switch (index.column()) {
    case IdHeader:
      return index.data(IdRole);
    case FirstNameHeader:
      return index.data(FirstNameRole);
    case LastNameHeader:
      return index.data(LastNameRole);
    default:
      return {};
  }
};

QVariant AuthorRestModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const {
  CHECK_HEADERDATA(section, orientation);

  if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
    return QAbstractListModel::headerData(section, orientation, role);
  }

  switch (section) {
    case IdHeader:
      return tr("Id");
    case FirstNameHeader:
      return tr("First Name");
    case LastNameHeader:
      return tr("Last Name");
    default:
      return {};
  }
}
