#ifndef SCHEMA_H
#define SCHEMA_H

#include "schema/author.h"
#include "schema/book.h"
#include "schema/bookauthor.h"
#include "schema/category.h"

class Schema {
public:
  virtual ~Schema() = default;
  virtual QVariant createResource() const;
};

struct BookDetails : Book {
  QList<Author> authors;
  QList<Category> categories;

  static BookDetails fromJson(const QJsonObject &json);
};

struct AuthorDetails : Author {
  QList<Book> books;
  static AuthorDetails fromJson(const QJsonObject &json);
};

#endif /* !SCHEMA_H */
