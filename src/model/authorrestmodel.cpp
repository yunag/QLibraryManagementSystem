#include <QIcon>
#include <QJsonArray>

#include <QMimeData>

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
  m_authors =
    algorithm::transform<QList<AuthorItem>>(authors, [](auto authorJson) {
    AuthorItem author = Author::fromJson(authorJson.toObject());
    return author;
  });

  fetchMore({});
};

int AuthorRestModel::columnCount(const QModelIndex &parent) const {
  CHECK_COLUMNCOUNT(parent);
  return LastHeader + 1;
}

int AuthorRestModel::rowCount(const QModelIndex &parent) const {
  CHECK_ROWCOUNT(parent);
  return m_authorsCount;
}

bool AuthorRestModel::removeRows(int row, int count,
                                 const QModelIndex &parent) {
  CHECK_REMOVEROWS(row, count, parent);

  beginRemoveRows({}, row, row + count - 1);

  m_authors.remove(row, count);
  m_authorsCount -= count;

  endRemoveRows();

  return true;
}

bool AuthorRestModel::canFetchMore(const QModelIndex &parent) const {
  CHECK_CANFETCHMORE(parent);

  return m_authorsCount < m_authors.size();
}

void AuthorRestModel::fetchMore(const QModelIndex &parent) {
  CHECK_FETCHMORE(parent);

  static const int k_fetchSize = 10;

  int numItemsLeft = static_cast<int>(m_authors.size()) - m_authorsCount;
  int numItemsFetch = qMin(k_fetchSize, numItemsLeft);

  beginInsertRows({}, m_authorsCount, m_authorsCount + numItemsFetch - 1);

  m_authorsCount += numItemsFetch;

  for (int row = m_authorsCount - numItemsFetch; row < m_authorsCount; ++row) {
    const auto &author = m_authors.at(row);

    auto busyIndicator = ResourceManager::busyIndicator();
    processImageUrl(row, author.imageUrl, busyIndicator, ImageRole);
  }

  endInsertRows();
}

void AuthorRestModel::reset() {
  beginResetModel();
  clearReplies();

  m_authors.clear();
  m_authorsCount = 0;
  endResetModel();
}

QVariant AuthorRestModel::data(const QModelIndex &index, int role) const {
  CHECK_DATA(index);

  const AuthorItem &author = m_authors.at(index.row());

  switch (role) {
    case Qt::DisplayRole:
      return dataForColumn(index);
    case ObjectRole:
      return QVariant::fromValue(author);
    case ImageRole:
      return author.image;
    case FlagsRole:
      return QVariant::fromValue(author.flags);
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

bool AuthorRestModel::setData(const QModelIndex &index, const QVariant &value,
                              int role) {
  CHECK_DATA(index);

  if (data(index, role) == value) {
    return false;
  }

  AuthorItem &author = m_authors[index.row()];

  switch (role) {
    case ImageRole:
      author.image = value.value<QPixmap>();
      break;
    case FlagsRole:
      author.flags = value.value<Qt::ItemFlags>();
      break;
    default:
      AbstractRestModel::setData(index, value, role);
      break;
  }

  emit dataChanged(index, index, {role});
  return true;
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
    return AbstractRestModel::headerData(section, orientation, role);
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

Qt::ItemFlags AuthorRestModel::flags(const QModelIndex &index) const {
  CHECK_FLAGS(index);
  auto flags = index.data(FlagsRole).value<Qt::ItemFlags>();

  if (m_imageReplies.contains(index)) {
    /* Allow selection only when item is fully loaded */
    flags &= ~Qt::ItemIsSelectable;
  }
  return flags;
}

QStringList AuthorRestModel::mimeTypes() const {
  QStringList types;
  types << AuthorRestModel::MimeType;
  return types;
}

QMimeData *AuthorRestModel::mimeData(const QModelIndexList &indexes) const {
  auto *mimeData = new QMimeData();
  QByteArray encodedData;

  QDataStream stream(&encodedData, QIODevice::WriteOnly);

  for (const QModelIndex &index : indexes) {
    if (index.isValid()) {
      stream << index.data(ObjectRole).value<AuthorItem>();
    }
  }

  mimeData->setData(AuthorRestModel::MimeType, encodedData);
  return mimeData;
}

Qt::DropActions AuthorRestModel::supportedDropActions() const {
  return Qt::LinkAction | Qt::CopyAction;
}
