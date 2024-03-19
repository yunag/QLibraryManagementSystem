#ifndef BOOK_H
#define BOOK_H

#include <QDate>

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

  ~Book(){};
};

#endif  // BOOK_H
