#ifndef LOADINGMODEL_H
#define LOADINGMODEL_H

#include <QHash>
#include <QMovie>
#include <QStandardItemModel>

#include <QFuture>
#include <QFutureSynchronizer>

#include "network/network.h"

class LoadingModel : public QStandardItemModel {
  Q_OBJECT

public:
  explicit LoadingModel(QObject *parent = nullptr);

  void clear();

  void appendRow(QStandardItem *item, const QUrl &url,
                 QSharedPointer<QMovie> movie);
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;

private:
  struct Data {
    QSharedPointer<QMovie> movie;
    QFuture<QPixmap> future;
    ReplyPointer reply;
  };

  QHash<QPersistentModelIndex, Data> m_data;
};

#endif  // LOADINGMODEL_H
