#include <QHttpMultiPart>

#include <QFile>
#include <QFileInfo>
#include <QJsonObject>
#include <QMimeDatabase>

#include "network/network.h"

#include "common/json.h"

#include "bookcontroller.h"
#include "libraryapplication.h"

QHttpPart createHttpPart(QHttpMultiPart *multiPart, const QString &name,
                         const QByteArray &body) {
  QHttpPart httpPart;
  QString dispositionHeader = "form-data; name=\"%1\"";

  httpPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                     dispositionHeader.arg(name));
  httpPart.setBody(body);
  multiPart->append(httpPart);

  return httpPart;
}

QHttpPart createHttpFilePart(QHttpMultiPart *multiPart, const QString &name,
                             const QUrl &url) {
  QString filePath = url.toLocalFile();
  auto *file = new QFile(filePath);

  QFileInfo fileInfo(filePath);
  QMimeDatabase mimeDatabase;
  QString mimeType = mimeDatabase.mimeTypeForFile(fileInfo).name();

  QHttpPart httpPart;
  httpPart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
  httpPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                     QStringLiteral(R"(form-data; name="%1"; filename="%2")")
                       .arg(name)
                       .arg(fileInfo.fileName()));

  file->open(QIODeviceBase::ReadOnly);
  httpPart.setBodyDevice(file);
  file->setParent(multiPart);

  multiPart->append(httpPart);
  return httpPart;
}

QHttpMultiPart *createMultiPart(const Book &book) {
  auto *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

  createHttpPart(multiPart, "title", book.title.toUtf8());
  if (!book.description.isNull()) {
    createHttpPart(multiPart, "description", book.description.toUtf8());
  }
  if (book.coverUrl.isValid()) {
    createHttpFilePart(multiPart, "cover", book.coverUrl);
  }
  if (!book.publicationDate.isNull()) {
    createHttpPart(multiPart, "publication_date",
                   book.publicationDate.toString(Qt::ISODate).toUtf8());
  }
  createHttpPart(multiPart, "copies_owned",
                 QByteArray::number(book.copiesOwned));

  return multiPart;
}

QFuture<quint32> BookController::createBook(const Book &book) {
  RestApiManager *manager = App->network();

  QHttpMultiPart *multiPart = createMultiPart(book);

  auto [future, reply] = manager->post("/api/books", multiPart);

  return future.then([](const QByteArray &data) {
    QJsonDocument json = *json::byteArrayToJson(data);
    return json["id"].toVariant().toUInt();
  });
}

QFuture<BookData> BookController::getBookById(quint32 bookId) {
  RestApiManager *manager = App->network();

  auto [future, reply] = manager->get("/api/books/" + QString::number(bookId));

  return future.then([](const QByteArray &data) {
    QJsonObject json = json::byteArrayToJson(data)->object();

    return BookData::fromJson(json);
  });
}

QFuture<QByteArray> BookController::updateBook(const Book &book) {
  RestApiManager *manager = App->network();

  QHttpMultiPart *multiPart = createMultiPart(book);

  auto [future, reply] =
    manager->put("/api/books/" + QString::number(book.id), multiPart);
  return future;
}

QFuture<QByteArray> BookController::deleteBookById(quint32 bookId) {
  RestApiManager *manager = App->network();

  auto [future, reply] =
    manager->deleteResource("/api/books/" + QString::number(bookId));

  return future;
}
