#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "authordetailsdao.h"
#include "librarydatabase.h"

const QString AuthorDetailsDAO::kQuery = R"(
SELECT
  a.first_name,
  a.last_name,
  COALESCE(
    (
      SELECT
        JSON_ARRAYAGG(
          JSON_OBJECT(
            'title',
            b.title,
            'id',
            b.book_id,
            'cover_path',
            b.cover_path
          )
        )
      FROM
        book AS b
        INNER JOIN book_author ba ON ba.book_id = b.book_id
      WHERE
        ba.author_id = a.author_id
      ORDER BY
        b.title
    ),
    JSON_ARRAY()
  ) AS books
FROM
  author a
WHERE
  a.author_id = :author_id
)";

AuthorDetailsDAO::AuthorDetailsDAO(QObject *parent) : QObject{parent} {}

QFuture<AuthorDetails> AuthorDetailsDAO::fetchDetails(quint32 authorId) {
  return LibraryDatabase::exec(kQuery, {{":author_id", authorId}})
    .then(this, [authorId](const LibraryTable &table) {
      AuthorDetails authorDetails;

      Author &author = authorDetails.author;
      QList<AuthorDetails::BookInfo> &books = authorDetails.books;

      const QVariantList &row = table[0];

      author.author_id = authorId;
      author.first_name = row[0].toString();
      author.last_name = row[1].toString();

      QString booksJson = row[2].toString();

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

      return authorDetails;
    });
}
