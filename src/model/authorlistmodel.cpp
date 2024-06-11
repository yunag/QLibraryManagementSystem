#include <QMimeData>

#include "authorlistmodel.h"
#include "authorrestmodel.h"

bool AuthorListModel::hasAuthorId(quint32 id) {
  for (int row = 0; row < rowCount(); ++row) {
    quint32 authorId = data(index(row, 0), AuthorRestModel::IdRole).toUInt();
    if (id == authorId) {
      return true;
    }
  }
  return false;
}

bool AuthorListModel::canDropMimeData(const QMimeData *data,
                                      Qt::DropAction action, int /*row*/,
                                      int column,
                                      const QModelIndex &parent) const {
  if (data->hasFormat(AuthorRestModel::MimeType)) {
    return action == Qt::CopyAction;
  }

  if (parent.isValid()) {
    return false;
  }

  return true;
}

bool AuthorListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                                   int row, int column,
                                   const QModelIndex &parent) {
  if (!data->hasFormat(AuthorRestModel::MimeType)) {
    return QStandardItemModel::dropMimeData(data, action, row, column, parent);
  }

  if (!canDropMimeData(data, action, row, column, parent)) {
    return false;
  }

  if (action == Qt::IgnoreAction) {
    return true;
  }

  QByteArray encodedData = data->data(AuthorRestModel::MimeType);
  QDataStream stream(&encodedData, QIODevice::ReadOnly);

  while (!stream.atEnd()) {
    AuthorItem author;
    stream >> author;

    auto *item =
      new QStandardItem(author.image, author.firstName + " " + author.lastName);
    item->setData(author.id, AuthorRestModel::IdRole);
    item->setData(QVariant::fromValue(author), AuthorRestModel::ObjectRole);
    item->setTextAlignment(Qt::AlignCenter);

    appendRow(item);
  }

  return true;
}
