#include <QJsonArray>
#include <QJsonObject>

#include "libraryapplication.h"
#include "network/httpmultipart.h"

#include "common/algorithm.h"

#include "schema/schema.h"

#define DEBUG_NETWORK_IMAGES

#ifdef DEBUG_NETWORK_IMAGES
  #include <QRandomGenerator>

const QList<QString> g_authorImages = {
  "https://upload.wikimedia.org/wikipedia/commons/5/56/Kiprensky_Pushkin.jpg",
  "https://upload.wikimedia.org/wikipedia/commons/2/24/Mikhail_lermontov.jpg",
  "https://upload.wikimedia.org/wikipedia/commons/a/a2/Shakespeare.jpg",
  "https://upload.wikimedia.org/wikipedia/commons/1/1c/"
  "L._N._Tolstoy%2C_by_Prokudin-Gorsky_%28cropped%29.jpg",
  "https://upload.wikimedia.org/wikipedia/commons/b/b2/"
  "N.Gogol_by_F.Moller_%281840%2C_Tretyakov_gallery%29.jpg",
  "https://upload.wikimedia.org/wikipedia/commons/thumb/2/2d/"
  "Jack_London_young.jpg/1200px-Jack_London_young.jpg"};

const QList<QString> g_bookCovers = {
  "https://book-cover.ru/sites/default/files/field/image/"
  "bulgakov-master-i-margarita.jpg",
  "https://images-na.ssl-images-amazon.com/images/S/"
  "compressed.photo.goodreads.com/books/1661678127i/929782.jpg",
  "https://www.litres.ru/pub/c/cover/67597292.jpg",
  "https://m.media-amazon.com/images/I/91FXycpulgL.jpg",
  "https://m.media-amazon.com/images/I/811buayzihL._AC_UF1000,1000_QL80_.jpg",
  "https://images-na.ssl-images-amazon.com/images/S/"
  "compressed.photo.goodreads.com/books/1500379840i/35699179.jpg"};
#endif /* DEBUG_NETWORK_IMAGES */

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

#ifdef DEBUG_NETWORK_IMAGES
  qint64 index = QRandomGenerator::global()->bounded(g_authorImages.length());
  book.coverUrl = g_bookCovers[index];
#endif /* DEBUG_NETWORK_IMAGES */

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
  author.imageUrl = json["image_url"].toString();

  if (!json["image_url"].isNull()) {
    QString path = "/" + json["image_url"].toString();
    QUrl imageUrl = ResourceManager::networkManager()->url();
    imageUrl.setPath(path);
    author.imageUrl = imageUrl;
  }

#ifdef DEBUG_NETWORK_IMAGES
  qint64 index = QRandomGenerator::global()->bounded(g_authorImages.length());
  author.imageUrl = g_authorImages[index];
#endif /* DEBUG_NETWORK_IMAGES */

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
