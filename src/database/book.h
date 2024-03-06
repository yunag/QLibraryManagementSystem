#ifndef BOOK_H
#define BOOK_H

#include <QDate>
#include <QFuture>
#include <QObject>

#include "schema.h"

class Book : public Schema {
  Q_GADGET

public:
  SCHEMA_NAME(book);

  SCHEMA_FIELD(QString, title);
  SCHEMA_FIELD(QString, description);
  SCHEMA_FIELD(QDate, publication_date);
  SCHEMA_FIELD(QString, cover_path);
  SCHEMA_FIELD(quint32, copies_owned);

  SCHEMA_KEY(quint32, book_id);

  Book() {}

  Book(QString title, QString description, QDate publication_date,
       QString cover_path, quint32 copies_owned, quint32 book_id = 0)
      : title(std::move(title)), description(std::move(description)),
        publication_date(publication_date), cover_path(std::move(cover_path)),
        copies_owned(copies_owned), book_id(book_id) {}

  ~Book(){};
};

#endif  // BOOK_H