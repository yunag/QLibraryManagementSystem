#include "bookresttableproxymodel.h"
#include "bookrestmodel.h"

QVariant BookRestTableProxyModel::headerData(int section,
                                             Qt::Orientation orientation,
                                             int role) const {
  if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
    return QIdentityProxyModel::headerData(section, orientation, role);
  }

  switch (section) {
    case 0:
      return tr("Book Id");
    case 1:
      return tr("Title");
    case 2:
      return tr("Rating");
    default:
      return "Invalid";
  }
}

QVariant
BookRestTableProxyModel::dataForColumn(int col,
                                       const QModelIndex &sourceIndex) {
  switch (col) {
    case 0:
      return sourceIndex.data(BookRestModel::IdRole);
    case 1:
      return sourceIndex.data(BookRestModel::TitleRole);
    case 2:
      return sourceIndex.data(BookRestModel::RatingRole);
    default:
      return {};
  }
};

QModelIndex BookRestTableProxyModel::index(int row, int column,
                                           const QModelIndex &parent) const {
  if (parent.isValid()) {
    return {};
  }
  return createIndex(row, column);
}

int BookRestTableProxyModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid()) {
    return 0;
  }
  /* id, title, rating */
  return 3;
}

QVariant BookRestTableProxyModel::data(const QModelIndex &proxyIndex,
                                       int role) const {
  QModelIndex sourceIndex = mapToSource(proxyIndex);

  QVariant displayData = dataForColumn(proxyIndex.column(), sourceIndex);

  switch (role) {
    case Qt::DisplayRole:
      return displayData;

    default:
      return QIdentityProxyModel::data(proxyIndex, role);
  }
}
