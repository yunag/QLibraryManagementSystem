#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>

#include <QFuture>

#include "network/networkerror.h"

using FutureReplyData = QFuture<QByteArray>;
using ReplyPointer = std::shared_ptr<QNetworkReply>;

struct NetworkFutureData {
  FutureReplyData future;
  ReplyPointer reply;
};

using FutureReply = NetworkFutureData;

class RestApiManager : public QNetworkAccessManager {
public:
  using QNetworkAccessManager::deleteResource;
  using QNetworkAccessManager::get;
  using QNetworkAccessManager::post;
  using QNetworkAccessManager::put;

public:
  explicit RestApiManager(QObject *parent = nullptr);

  void setUrl(const QUrl &url);
  QUrl url() const;

  void setBearerToken(const QByteArray &token);
  QNetworkRequest restRequest(const QString &api, const QUrlQuery &query = {});

  FutureReply post(const QString &api, const QVariantMap &value = {});
  FutureReply post(const QString &api, QHttpMultiPart *multiPart);
  FutureReply get(const QString &api, const QUrlQuery &parameters = {});
  FutureReply get(const QNetworkRequest &request);
  FutureReply put(const QString &api, const QVariantMap &value);
  FutureReply put(const QString &api, QHttpMultiPart *multiPart);
  FutureReply deleteResource(const QString &api);

private:
  static NetworkError httpResponseSuccess(QNetworkReply *reply);
  static FutureReply futureReply(QNetworkReply *reply);

private:
  QUrl m_url;
  QByteArray m_bearerToken;
};

#endif  // NETWORK_H
