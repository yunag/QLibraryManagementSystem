#ifndef BOOKAUTHOR_H
#define BOOKAUTHOR_H

#include <QFuture>

#include "schema.h"

class BookAuthor : public Schema {
  Q_GADGET

public:
  BookAuthor(quint32 book_id, quint32 author_id)
      : book_id(book_id), author_id(author_id) {}

  SCHEMA_NAME(book_author);
  SCHEMA_KEY(quint32, book_id);
  SCHEMA_FIELD(quint32, author_id);

  static QFuture<void> update(quint32 book_id,
                              const QList<quint32> &author_ids);
};
#endif /* !BOOKAUTHOR_H */
