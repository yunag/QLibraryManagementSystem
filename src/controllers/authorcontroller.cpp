#include <QJsonArray>
#include <QJsonObject>

#include "common/json.h"
#include "network/network.h"

#include "authorcontroller.h"
#include "libraryapplication.h"

QFuture<QList<Author>> AuthorController::getAuthors() {
  RestApiManager *manager = App->network();

  auto [future, reply] = manager->get("/api/authors");

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
