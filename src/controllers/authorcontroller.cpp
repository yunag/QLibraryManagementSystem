#include <QJsonArray>
#include <QJsonObject>

#include "common/json.h"
#include "network/network.h"

#include "authorcontroller.h"
#include "resourcemanager.h"

QFuture<QList<Author>> AuthorController::getAuthors() {
  RestApiManager *manager = ResourceManager::networkManager();

  QUrlQuery params;
  params.addQueryItem("perpage", QString::number(300));
  auto [future, reply] = manager->get("/api/authors", params);

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

QFuture<quint32> AuthorController::createAuthor(const Author &author) {
  RestApiManager *manager = ResourceManager::networkManager();

  QHttpMultiPart *multiPart = author.createHttpMultiPart();

  auto [future, reply] = manager->post("/api/authors", multiPart);

  return future.then([](const QByteArray &data) {
    QJsonDocument json = *json::byteArrayToJson(data);
    return json["id"].toVariant().toUInt();
  });
}
