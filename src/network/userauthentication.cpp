#include "common/json.h"

#include "network/network.h"
#include "network/userauthentication.h"

QFuture<void> UserAuthentication::login(const QUrl &host,
                                        const QString &username,
                                        const QString &password) {
  m_manager->setUrl(host);

  QVariantMap userFields;
  userFields["username"] = username;
  userFields["password"] = password;

  auto [future, reply] = m_manager->post("/api/login", userFields);

  return future.then(QtFuture::Launch::Async, [this](const QByteArray &data) {
    QJsonDocument json = *json::byteArrayToJson(data);
    QString token = json["token"].toString();

    m_manager->setBearerToken(token.toUtf8());
  });
}

QFuture<void> UserAuthentication::registerUser(const QUrl &host,
                                               const QString &username,
                                               const QString &password) {
  m_manager->setUrl(host);

  QVariantMap userFields;
  userFields["username"] = username;
  userFields["password"] = password;

  auto [future, reply] = m_manager->post("/api/register", userFields);

  return future.then(QtFuture::Launch::Async,
                     [](const QByteArray &data) { qInfo() << data; });
}
