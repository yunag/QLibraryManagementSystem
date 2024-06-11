#include <QJsonArray>
#include <QJsonObject>

#include "libraryapplication.h"
#include "network/httpmultipart.h"

#include "common/algorithm.h"

#include "schema/schema.h"

Book Book::fromJson(const QJsonObject &json) {
  Book book;

  book.id = json["id"].toVariant().toUInt();
  book.publicationDate =
    QDate::fromString(json["publication_date"].toString(), "dd-MM-yyyy");
  book.copiesOwned = json["copies_owned"].toInt();
  book.description = json["description"].toString();
  book.title = json["title"].toString();
  book.rating = json["rating"].toDouble();

  if (!json["cover_url"].isNull()) {
    QString path = "/" + json["cover_url"].toString();
    QUrl coverUrl = ResourceManager::networkManager()->url();
    coverUrl.setPath(path);
    book.coverUrl = coverUrl;
  }
  /* TODO: Remove after testing */
  book.coverUrl = "https://static.wikia.nocookie.net/kiminonawa/images/6/62/"
                  "Kimi-no-Na-wa.-Visual.jpg/revision/latest?cb=20160927170951";
  return book;
}

BookDetails BookDetails::fromJson(const QJsonObject &json) {
  QJsonArray categoriesJson = json["categories"].toArray();
  QJsonArray authorsJson = json["authors"].toArray();

  auto categories = algorithm::transform<QList<Category>>(
    categoriesJson, [](auto categoryJson) {
    return Category::fromJson(categoryJson.toObject());
  });
  auto authors =
    algorithm::transform<QList<Author>>(authorsJson, [](auto authorJson) {
    return Author::fromJson(authorJson.toObject());
  });

  return {Book::fromJson(json), authors, categories};
}

Author Author::fromJson(const QJsonObject &json) {
  Author author;

  author.id = json["id"].toVariant().toUInt();
  author.firstName = json["first_name"].toString();
  author.lastName = json["last_name"].toString();
  author.imageUrl = json["last_name"].toString();

  /* TODO: Remove after testing */
  author.imageUrl =
    "https://static.wikia.nocookie.net/kiminonawa/images/6/62/"
    "Kimi-no-Na-wa.-Visual.jpg/revision/latest?cb=20160927170951";

  return author;
}

AuthorDetails AuthorDetails::fromJson(const QJsonObject &json) {
  QJsonArray booksJson = json["books"].toArray();

  auto books = algorithm::transform<QList<Book>>(booksJson, [](auto bookJson) {
    return Book::fromJson(bookJson.toObject());
  });

  return {Author::fromJson(json), books};
}

Category Category::fromJson(const QJsonObject &json) {
  Category category;

  category.id = json["id"].toVariant().toUInt();
  category.name = json["name"].toString();

  return category;
}

QHttpMultiPart *Book::createHttpMultiPart() const {
  auto *multiPart = new HttpMultiPart(QHttpMultiPart::FormDataType);

  if (!title.isNull()) {
    multiPart->addPart("title", title.toUtf8());
  }
  if (!description.isNull()) {
    multiPart->addPart("description", description.toUtf8());
  }
  if (coverUrl.isValid()) {
    multiPart->addFilePart("cover", coverUrl.toLocalFile());
  }
  if (!publicationDate.isNull()) {
    multiPart->addPart("publication_date",
                       publicationDate.toString(Qt::ISODate).toUtf8());
  }

  multiPart->addPart("copies_owned", QByteArray::number(copiesOwned));

  return multiPart;
}

QHttpMultiPart *Author::createHttpMultiPart() const {
  auto *multiPart = new HttpMultiPart(QHttpMultiPart::FormDataType);

  if (!firstName.isNull()) {
    multiPart->addPart("first_name", firstName.toUtf8());
  }
  if (!lastName.isNull()) {
    multiPart->addPart("last_name", lastName.toUtf8());
  }
  if (imageUrl.isValid()) {
    multiPart->addFilePart("avatar", imageUrl.toLocalFile());
  }

  return multiPart;
}

QVariant Schema::createResource() const {}
