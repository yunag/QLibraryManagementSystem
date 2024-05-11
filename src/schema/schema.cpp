#include <QJsonArray>
#include <QJsonObject>

#include "libraryapplication.h"

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
    QUrl coverUrl = App->network()->url();
    coverUrl.setPath(path);
    bookData.coverUrl = coverUrl;
  }

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

  return author;
}

Category Category::fromJson(const QJsonObject &json) {
  Category category;

  category.id = json["id"].toVariant().toUInt();
  category.name = json["name"].toString();

  return category;
}