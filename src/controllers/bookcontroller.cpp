#include <QHttpMultiPart>

#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QMimeDatabase>

#include "network/httpmultipart.h"
#include "network/network.h"

#include "common/json.h"

#include "bookcontroller.h"
#include "resourcemanager.h"

QFuture<quint32> BookController::createBook(const Book &book) {
  RestApiManager *manager = ResourceManager::networkManager();

  QHttpMultiPart *multiPart = book.createHttpMultiPart();

  auto [future, reply] = manager->post("/api/books", multiPart);

  return future.then([](const QByteArray &data) {
    QJsonDocument json = *json::byteArrayToJson(data);
    return json["id"].toVariant().toUInt();
  });
}

QFuture<BookData> BookController::getBookById(quint32 bookId) {
  RestApiManager *manager = ResourceManager::networkManager();

  auto [future, reply] = manager->get("/api/books/" + QString::number(bookId));

  return future.then([](const QByteArray &data) {
    QJsonObject json = json::byteArrayToJson(data)->object();

    return BookData::fromJson(json);
  });
}

QFuture<QByteArray> BookController::updateBook(const Book &book) {
  RestApiManager *manager = ResourceManager::networkManager();

  QHttpMultiPart *multiPart = book.createHttpMultiPart();

  auto [future, reply] =
    manager->put("/api/books/" + QString::number(book.id), multiPart);
  return future;
}

QFuture<QByteArray> BookController::deleteBookById(quint32 bookId) {
  RestApiManager *manager = ResourceManager::networkManager();

  auto [future, reply] =
    manager->deleteResource("/api/books/" + QString::number(bookId));

  return future;
}
