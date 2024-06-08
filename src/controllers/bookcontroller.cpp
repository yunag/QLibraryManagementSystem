#include <QHttpMultiPart>

#include <QJsonObject>

#include "network/network.h"

#include "common/json.h"

#include "bookcontroller.h"

QFuture<quint32> BookController::create(const Book &book) {
  QHttpMultiPart *multiPart = book.createHttpMultiPart();

  auto [future, reply] = m_manager->post("/api/books", multiPart);

  return future.then([](const QByteArray &data) {
    QJsonDocument json = *json::byteArrayToJson(data);
    return json["id"].toVariant().toUInt();
  });
}

QFuture<BookDetails> BookController::get(quint32 id) {
  auto [future, reply] = m_manager->get("/api/books/" + QString::number(id));

  return future.then([](const QByteArray &data) {
    QJsonObject json = json::byteArrayToJson(data)->object();

    return BookDetails::fromJson(json);
  });
}

QFuture<void> BookController::update(quint32 id, const Book &book) {
  QHttpMultiPart *multiPart = book.createHttpMultiPart();

  auto [future, reply] =
    m_manager->put("/api/books/" + QString::number(id), multiPart);
  return future.then([](auto) {});
}

QFuture<void> BookController::deleteResource(quint32 id) {
  auto [future, reply] =
    m_manager->deleteResource("/api/books/" + QString::number(id));

  return future.then([](auto) {});
}
