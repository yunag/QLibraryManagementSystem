#include <QJsonArray>
#include <QJsonObject>

#include "libraryapplication.h"
#include "network/httpmultipart.h"

#include "schema/schema.h"

BookData BookData::fromJson(const QJsonObject &json) {
  BookData bookData;

  bookData.id = json["id"].toVariant().toUInt();
  bookData.publicationDate =
    QDate::fromString(json["publication_date"].toString(), "dd-MM-yyyy");
  bookData.copiesOwned = json["copies_owned"].toInt();
  bookData.description = json["description"].toString();
  bookData.title = json["title"].toString();
  bookData.rating = json["rating"].toDouble();

  if (!json["cover_url"].isNull()) {
    QString path = "/" + json["cover_url"].toString();
    QUrl coverUrl = ResourceManager::networkManager()->url();
    coverUrl.setPath(path);
    bookData.coverUrl = coverUrl;
  }
  /* TODO: Remove after testing */
  bookData.coverUrl =
    "https://static.wikia.nocookie.net/kiminonawa/images/6/62/"
    "Kimi-no-Na-wa.-Visual.jpg/revision/latest?cb=20160927170951";

  QJsonArray categoriesJson = json["categories"].toArray();

  for (const auto categoryJson : categoriesJson) {
    Category category = Category::fromJson(categoryJson.toObject());

    bookData.categories.push_back(category);
  }

  QJsonArray authorsJson = json["authors"].toArray();

  for (const auto authorJson : authorsJson) {
    Author author = Author::fromJson(authorJson.toObject());

    bookData.authors.push_back(author);
  }

  return bookData;
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
