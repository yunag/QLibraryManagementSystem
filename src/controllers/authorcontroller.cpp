#include <QJsonArray>
#include <QJsonObject>

#include "common/json.h"
#include "network/network.h"

#include "authorcontroller.h"

QFuture<QList<Author>> AuthorController::getAuthors() {
  QUrlQuery params;
  params.addQueryItem("perpage", QString::number(300));
  auto [future, reply] = m_manager->get("/api/authors", params);

  return future.then([](const QByteArray &data) {
    QJsonArray json = json::byteArrayToJson(data)->array();

    QList<Author> authors;
    for (const auto authorJson : json) {
      Author author = Author::fromJson(authorJson.toObject());

      authors.push_back(author);
    }
    return authors;
  });
}

QFuture<quint32> AuthorController::create(const Author &author) {
  QHttpMultiPart *multiPart = author.createHttpMultiPart();

  auto [future, reply] = m_manager->post("/api/authors", multiPart);

  return future.then([](const QByteArray &data) {
    QJsonDocument json = *json::byteArrayToJson(data);
    return json["id"].toVariant().toUInt();
  });
}

QFuture<AuthorDetails> AuthorController::get(quint32 id) {
  auto [future, reply] = m_manager->get("/api/authors/" + QString::number(id));

  return future.then([](const QByteArray &data) {
    QJsonObject json = json::byteArrayToJson(data)->object();

    return AuthorDetails::fromJson(json);
  });
}

QFuture<void> AuthorController::update(quint32 id, const Author &author) {
  QHttpMultiPart *multiPart = author.createHttpMultiPart();

  auto [future, reply] =
    m_manager->put("/api/authors/" + QString::number(id), multiPart);
  return future.then([](auto) {});
}

QFuture<void> AuthorController::deleteResource(quint32 id) {
  auto [future, reply] =
    m_manager->deleteResource("/api/authors/" + QString::number(id));

  return future.then([](auto) {});
}
