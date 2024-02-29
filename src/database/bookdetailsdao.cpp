#include "bookdetailsdao.h"
#include "librarydatabase.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

const QString BookDetailsDAO::kQuery = R"(
SELECT
  b.title,
  b.description,
  b.publication_date,
  b.copies_owned,
  COALESCE(
    (
      SELECT
        JSON_ARRAYAGG(
          JSON_OBJECT(
            'first_name',
            a.first_name,
            'last_name',
            a.last_name,
            'id',
            a.author_id
          )
        )
      FROM
        author AS a
        INNER JOIN book_author ba ON ba.author_id = a.author_id
      WHERE
        ba.book_id = b.book_id
    ),
    JSON_ARRAY()
  ) AS authors,
  COALESCE(
    (
      SELECT
        JSON_ARRAYAGG(JSON_OBJECT('name', c.name, 'id', c.category_id))
      FROM
        category AS c
        INNER JOIN book_category bc ON bc.category_id = c.category_id
      WHERE
        bc.book_id = b.book_id
    ),
    JSON_ARRAY()
  ) AS categories
FROM
  book b
WHERE
  b.book_id = :book_id
)";

BookDetailsDAO::BookDetailsDAO(QObject *parent) : QObject{parent} {}

QFuture<BookDetails> BookDetailsDAO::fetchDetails(quint32 bookId) {
  return LibraryDatabase::exec(kQuery, {{":book_id", bookId}})
    .then([](const LibraryTable &table) {
      BookDetails bookDetails;

      Q_ASSERT(table.size() == 1);

      const QVariantList &row = table[0];

      Book &book = bookDetails.book;

      book.title = row[0].toString();
      book.description = row[1].toString();
      book.publication_date = row[2].toDate();
      book.copies_owned = row[3].toUInt();

      QString authorsJson = row[4].toString();
      QString categoriesJson = row[5].toString();

      QJsonDocument authorsDocument =
        QJsonDocument::fromJson(authorsJson.toUtf8());
      QJsonArray authorsJsonArray = authorsDocument.array();

      for (QJsonValueRef ref : authorsJsonArray) {
        QJsonObject obj = ref.toObject();

        Author author;
        author.first_name = obj["first_name"].toString();
        author.last_name = obj["last_name"].toString();
        author.author_id = obj["id"].toInt();

        bookDetails.authors.push_back(std::move(author));
      }

      QJsonDocument categoriesDocument =
        QJsonDocument::fromJson(categoriesJson.toUtf8());
      QJsonArray categoriesJsonArray = categoriesDocument.array();

      for (QJsonValueRef ref : categoriesJsonArray) {
        QJsonObject obj = ref.toObject();

        Category category;
        category.name = obj["name"].toString();
        category.category_id = obj["id"].toInteger();

        bookDetails.categories.push_back(std::move(category));
      }

      return bookDetails;
    });
}
