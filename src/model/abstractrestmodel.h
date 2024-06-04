#ifndef RESTMODEL_H
#define RESTMODEL_H

#include <QAbstractListModel>
#include <QMovie>

#include "network/network.h"
#include "pagination.h"

class Pagination;

class AbstractRestModel : public QAbstractListModel {
  Q_OBJECT

  Q_PROPERTY(QVariantMap filters READ filters WRITE setFilters NOTIFY
               filtersChanged FINAL)
  Q_PROPERTY(Pagination *pagination READ pagination WRITE setPagination NOTIFY
               paginationChanged FINAL)
  Q_PROPERTY(
    QString orderBy READ orderBy WRITE setOrderBy NOTIFY orderByChanged FINAL)
  Q_PROPERTY(RestApiManager *restManager READ restManager WRITE setRestManager
               NOTIFY restManagerChanged FINAL)

public:
  using QAbstractListModel::QAbstractListModel;

  void reload();

  virtual QUrlQuery includeFieldsQuery() { return {}; }

  virtual void handleRequestData(const QByteArray &data) = 0;

  void abortReplies();
  void setRoute(const QString &route);

  RestApiManager *restManager() const;
  void setRestManager(RestApiManager *newRestManager);

  QVariantMap filters() const;
  void setFilters(const QVariantMap &newFilters);

  Pagination *pagination() const;
  void setPagination(Pagination *newPagination);

  QString orderBy() const;
  void setOrderBy(const QString &newOrderBy);

signals:
  void reloadFinished();
  void restManagerChanged();
  void filtersChanged();
  void paginationChanged();
  void orderByChanged();

protected:
  void updatePagination(ReplyPointer reply);
  void processImageUrl(int row, const QUrl &url,
                       QSharedPointer<QMovie> busyIndicator = nullptr,
                       int role = Qt::DecorationRole);

protected:
  RestApiManager *m_restManager = nullptr;
  Pagination *m_pagination = nullptr;
  ReplyPointer m_reply = nullptr;

  QHash<QPersistentModelIndex, ReplyPointer> m_imageReplies;

  QUrlQuery m_baseQuery;
  QVariantMap m_filters;
  QString m_orderBy;
  QString m_route;
};

#endif /* !RESTMODEL_H */
