#include "bookresttableproxymodel.h"
#include "bookrestmodel.h"

QVariant BookRestTableProxyModel::headerData(int section,
                                             Qt::Orientation orientation,
                                             int role) const {
  if (role != Qt::DisplayRole || orientation == Qt::Vertical) {
    return QIdentityProxyModel::headerData(section, orientation, role);
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

QVariant
BookRestTableProxyModel::dataForColumn(int col,
                                       const QModelIndex &sourceIndex) {
  switch (col) {
    case Id:
      return sourceIndex.data(BookRestModel::IdRole);
    case Title:
      return sourceIndex.data(BookRestModel::TitleRole);
    case Authors:
      return sourceIndex.data(BookRestModel::AuthorsRole)
        .toStringList()
        .join(", ");
    case Categories:
      return sourceIndex.data(BookRestModel::CategoriesRole)
        .toStringList()
        .join(", ");
    case Rating:
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

  return LastSection + 1;
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
