#ifndef BOOKCONTROLLER_H
#define BOOKCONTROLLER_H

#include <QFuture>

#include "controller.h"

#include "schema/schema.h"

class BookController : public Controller {
public:
  QFuture<quint32> create(const Book &book);
  QFuture<BookDetails> get(quint32 id);
  QFuture<void> update(quint32 id, const Book &book);
  QFuture<void> deleteResource(quint32 id);
};

#endif /* !BOOKCONTROLLER_H */
