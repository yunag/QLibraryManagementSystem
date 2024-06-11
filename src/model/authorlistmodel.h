#ifndef AUTHORLISTMODEL_H
#define AUTHORLISTMODEL_H

#include <QStandardItemModel>

class AuthorListModel : public QStandardItemModel {
  Q_OBJECT

public:
  using QStandardItemModel::QStandardItemModel;

  bool hasAuthorId(quint32 id);

  bool canDropMimeData(const QMimeData *data, Qt::DropAction action,
                       int /*row*/, int column,
                       const QModelIndex &parent) const override;

  bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                    int column, const QModelIndex &parent) override;
};

#endif /* !AUTHORLISTMODEL_H */
