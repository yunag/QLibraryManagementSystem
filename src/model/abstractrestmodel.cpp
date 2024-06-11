#include "abstractrestmodel.h"

#include "network/imageloader.h"
#include "pagination.h"

void AbstractRestModel::reload() {
  QUrlQuery query = includeFieldsQuery();

  for (const auto &[filter, value] : m_filters.asKeyValueRange()) {
    if (value.userType() == QMetaType::QVariantList) {
      for (const auto &v : value.toList()) {
        query.addQueryItem(filter, v.toString());
      }
    } else {
      query.addQueryItem(filter, value.toString());
    }
  }

  query.addQueryItem("orderby", m_orderBy);
  query.addQueryItem("page", QString::number(m_pagination->currentPage()));
  query.addQueryItem("perpage", QString::number(m_pagination->perPage()));

  auto [future, reply] = m_restManager->get(m_route, query);

  m_reply = reply;

  future
    .then(this, [this](const QByteArray &data) {
    updatePagination(m_reply);

    handleRequestData(data);

    emit reloadFinished();
  }).onFailed(this, [](const NetworkError &err) { qDebug() << err.message(); });
}

void AbstractRestModel::updatePagination(ReplyPointer reply) {
  QByteArray currentPage = m_pagination->currentPageHeader().toUtf8();
  QByteArray totalCount = m_pagination->totalCountHeader().toUtf8();
  QByteArray pageCount = m_pagination->pageCountHeader().toUtf8();

  m_pagination->setCurrentPage(reply->rawHeader(currentPage).toInt());
  m_pagination->setTotalCount(reply->rawHeader(totalCount).toInt());
  Q_UNUSED(pageCount);
}

void AbstractRestModel::processImageUrl(int row, const QUrl &url,
                                        QSharedPointer<QMovie> busyIndicator,
                                        int role) {
  ImageLoader imageLoader(m_restManager);

  auto [future, reply] = imageLoader.load(url);

  QPersistentModelIndex persistentIndex = index(row);

  m_imageReplies[persistentIndex] = {reply};

  if (busyIndicator) {
    busyIndicator->start();

    connect(busyIndicator.get(), &QMovie::frameChanged, this,
            &AbstractRestModel::onBusyIndicatorFrameChanged,
            Qt::UniqueConnection);
  }

  auto handleImage = [this, persistentIndex, busyIndicator,
                      role](const QPixmap &pixmap) {
    if (m_imageReplies.contains(persistentIndex)) {
      setData(persistentIndex, pixmap, role);
      m_imageReplies.remove(persistentIndex);
    }
  };

  future
    .then(this, [handleImage](const QPixmap &pixmap) {
    handleImage(pixmap);
  }).onFailed(this, [handleImage](const NetworkError &err) {
    if (err.type() != QNetworkReply::OperationCanceledError) {
      QPixmap defaultCover(":/images/DefaultBookCover");
      handleImage(defaultCover);
    }
  });
}

void AbstractRestModel::onBusyIndicatorFrameChanged(int /*frame*/) {
  for (const auto &persistentIndex : m_imageReplies.keys()) {
    emit dataChanged(persistentIndex, persistentIndex);
  }
}

RestApiManager *AbstractRestModel::restManager() const {
  return m_restManager;
}

void AbstractRestModel::setRestManager(RestApiManager *newRestManager) {
  if (m_restManager == newRestManager)
    return;

  m_restManager = newRestManager;
  emit restManagerChanged();
}

QVariantMap AbstractRestModel::filters() const {
  return m_filters;
}

void AbstractRestModel::setFilters(const QVariantMap &newFilters) {
  if (m_filters == newFilters)
    return;

  m_filters = newFilters;
  emit filtersChanged();
}

Pagination *AbstractRestModel::pagination() const {
  return m_pagination;
}

void AbstractRestModel::setPagination(Pagination *newPagination) {
  if (m_pagination == newPagination)
    return;

  m_pagination = newPagination;

  emit paginationChanged();
}

QString AbstractRestModel::orderBy() const {
  return m_orderBy;
}

void AbstractRestModel::setOrderBy(const QString &newOrderBy) {
  if (m_orderBy == newOrderBy)
    return;

  m_orderBy = newOrderBy;
  emit orderByChanged();
}

void AbstractRestModel::setRoute(const QString &route) {
  m_route = route;
}

void AbstractRestModel::clearReplies() {
  m_imageReplies.clear();
  m_reply.reset();
}
