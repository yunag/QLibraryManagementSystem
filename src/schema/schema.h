#ifndef SCHEMA_H
#define SCHEMA_H

#include "schema/author.h"
#include "schema/book.h"
#include "schema/bookauthor.h"
#include "schema/category.h"

struct BookData : Book {
  QList<Author> authors;
  QList<Category> categories;

  static BookData fromJson(const QJsonObject &json);
};

struct AuthorData : Author {
  QList<Book> books;
};

#endif /* !SCHEMA_H */
