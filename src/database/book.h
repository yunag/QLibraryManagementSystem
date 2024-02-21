#ifndef BOOK_H
#define BOOK_H

#include <QFuture>
#include <QObject>

#include "schema.h"

class Book : public Schema {
  Q_GADGET

public:
  SCHEMA_NAME(book);
  SCHEMA_KEY(quint32, book_id);

  SCHEMA_FIELD(QString, title);
  SCHEMA_FIELD(QString, publication_date);
  SCHEMA_FIELD(QString, cover_path);
  SCHEMA_FIELD(quint32, copies_owned);

  Book(QString title, QString publication_date, QString cover_path,
       quint32 copies_owned, quint32 book_id = 0)
      : book_id(book_id), title(std::move(title)),
        publication_date(std::move(publication_date)),
        cover_path(std::move(cover_path)), copies_owned(copies_owned) {}

  ~Book(){};
};

#endif // BOOK_H
