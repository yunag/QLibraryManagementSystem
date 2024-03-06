#include "bookcardmodel.h"

BookCardModel::BookCardModel(QObject *parent) : QAbstractItemModel(parent) {}

QModelIndex BookCardModel::index(int row, int column,
                                 const QModelIndex &parent) const {
  // FIXME: Implement me!
}

QModelIndex BookCardModel::parent(const QModelIndex &index) const {
  // FIXME: Implement me!
}

int BookCardModel::rowCount(const QModelIndex &parent) const {
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

int BookCardModel::columnCount(const QModelIndex &parent) const {
  if (!parent.isValid())
    return 0;

  // FIXME: Implement me!
}

bool BookCardModel::hasChildren(const QModelIndex &parent) const {
  // FIXME: Implement me!
}

bool BookCardModel::canFetchMore(const QModelIndex &parent) const {
  // FIXME: Implement me!
  return false;
}

void BookCardModel::fetchMore(const QModelIndex &parent) {
  // FIXME: Implement me!
}

QVariant BookCardModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  // FIXME: Implement me!
  return QVariant();
}

bool BookCardModel::setData(const QModelIndex &index, const QVariant &value,
                            int role) {
  if (data(index, role) != value) {
    // FIXME: Implement me!
    emit dataChanged(index, index, {role});
    return true;
  }
  return false;
}

Qt::ItemFlags BookCardModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  return QAbstractItemModel::flags(index) |
         Qt::ItemIsEditable;  // FIXME: Implement me!
}
