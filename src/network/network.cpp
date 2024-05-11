#include <QJsonDocument>
#include <QJsonObject>

#include <QHttpMultiPart>
#include <QNetworkReply>

#include "common/json.h"
#include "network/network.h"

RestApiManager::RestApiManager(QObject *parent)
    : QNetworkAccessManager(parent) {}

static int httpStatusCode(QNetworkReply *reply) {
  return reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

static bool isGoodResponse(int httpCode) {
  return 200 <= httpCode && httpCode < 300;
}

static QString
requestMethodToString(QNetworkAccessManager::Operation operation) {
  switch (operation) {
    case QNetworkAccessManager::HeadOperation:
      return "HEAD";
    case QNetworkAccessManager::GetOperation:
      return "GET";
    case QNetworkAccessManager::PutOperation:
      return "PUT";
    case QNetworkAccessManager::PostOperation:
      return "POST";
    case QNetworkAccessManager::DeleteOperation:
      return "DELETE";
    case QNetworkAccessManager::CustomOperation:
      return "CUSTOM";
    case QNetworkAccessManager::UnknownOperation:
    default:
      return "UNKNOWN";
  }
}

NetworkError RestApiManager::httpResponseSuccess(QNetworkReply *reply) {
  const int httpCode = httpStatusCode(reply);
  const bool isReplyError = reply->error() != QNetworkReply::NoError;

  QString debugMessage = QString("Request[%1]: %2")
                           .arg(requestMethodToString(reply->operation()))
                           .arg(reply->request().url().path());

  qInfo().noquote() << debugMessage;

  if (isReplyError) {
    qWarning().noquote() << "\t[NetworkError]: " + reply->errorString();
  }

  QString errorMessage = "";
  if (!isGoodResponse(httpCode)) {
    QString response;
    QByteArray data = reply->readAll();

    if (reply->header(QNetworkRequest::ContentTypeHeader)
          .toString()
          .contains("application/json")) {
      QJsonDocument json = *json::byteArrayToJson(data);

      response = json["error"].toString();
    } else {
      response = data;
    }

    errorMessage = response;

    qWarning().noquote()
      << QString("\t[ServerError](http: %1): ").arg(httpCode) + response;
  }

  return {reply->error(), errorMessage};
}

FutureReply RestApiManager::post(const QString &api,
                                 QHttpMultiPart *multiPart) {
  QNetworkRequest request = restRequest(api);

  QNetworkReply *reply = QNetworkAccessManager::post(request, multiPart);
  multiPart->setParent(reply);

  return futureReply(reply);
}

FutureReply RestApiManager::post(const QString &api, const QVariantMap &value) {
  QNetworkRequest request = restRequest(api);

  request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader,
                    "application/json");

  QNetworkReply *reply = QNetworkAccessManager::post(
    request, QJsonDocument::fromVariant(value).toJson(QJsonDocument::Compact));

  return futureReply(reply);
}

FutureReply RestApiManager::get(const QString &api,
                                const QUrlQuery &parameters) {
  QNetworkRequest request = restRequest(api, parameters);

  QNetworkReply *reply = QNetworkAccessManager::get(request);

  return futureReply(reply);
}

FutureReply RestApiManager::get(const QNetworkRequest &request) {
  return futureReply(QNetworkAccessManager::get(request));
}

FutureReply RestApiManager::put(const QString &api, const QVariantMap &value) {
  QNetworkRequest request = restRequest(api);

  request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader,
                    "application/json");

  QNetworkReply *reply = QNetworkAccessManager::put(
    request, QJsonDocument::fromVariant(value).toJson(QJsonDocument::Compact));

  return futureReply(reply);
}

FutureReply RestApiManager::put(const QString &api, QHttpMultiPart *multiPart) {
  QNetworkRequest request = restRequest(api);

  QNetworkReply *reply = QNetworkAccessManager::put(request, multiPart);
  multiPart->setParent(reply);

  return futureReply(reply);
}

FutureReply RestApiManager::deleteResource(const QString &api) {
  QNetworkRequest request = restRequest(api);

  QNetworkReply *reply = QNetworkAccessManager::deleteResource(request);

  return futureReply(reply);
}

void RestApiManager::setUrl(const QUrl &url) {
  m_url = url;
}

void RestApiManager::setBearerToken(const QByteArray &token) {
  m_bearerToken = token;
}

FutureReply RestApiManager::futureReply(QNetworkReply *replyBase) {
  auto reply = std::shared_ptr<QNetworkReply>(
    replyBase, [](QObject *obj) { obj->deleteLater(); });

  auto future =
    QtFuture::connect(reply.get(), &QNetworkReply::finished).then([reply]() {
      if (reply->error() != NetworkErrorType::NoError) {
        throw httpResponseSuccess(reply.get());
      }

      QByteArray data;
      if (reply->isReadable()) {
        data = reply->readAll();
      }

      return data;
    });

  return {future, reply};
}

QUrl RestApiManager::url() const {
  return m_url;
};

QNetworkRequest RestApiManager::restRequest(const QString &api,
                                            const QUrlQuery &query) {
  QUrl url(m_url);
  url.setPath(api);

  if (!query.isEmpty()) {
    url.setQuery(query);
  }

  QNetworkRequest request(url);
  request.setRawHeader("Authorization", "Bearer " + m_bearerToken);

  return request;
}
