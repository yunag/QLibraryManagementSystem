#include "authordetailsdao.h"

AuthorDetailsDAO::AuthorDetailsDAO(QObject *parent) : QObject{parent} {}

QFuture<AuthorDetails> AuthorDetailsDAO::fetchDetails(quint32 authorId) {
  /* TODO: implement */

  /*
      QJsonDocument booksDocument = QJsonDocument::fromJson(booksJson.toUtf8());
      QJsonArray booksJsonArray = booksDocument.array();

      for (QJsonValueRef ref : booksJsonArray) {
        QJsonObject obj = ref.toObject();
        AuthorDetails::BookInfo book;

        book.title = obj["title"].toString();
        book.coverPath = obj["cover_path"].toString();
        book.id = obj["id"].toInt();

        books.push_back(std::move(book));
      }
    */
}
