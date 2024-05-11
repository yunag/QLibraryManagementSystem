#ifndef BOOKCONTROLLER_H
#define BOOKCONTROLLER_H

#include <QFuture>

#include "schema/schema.h"

class BookController {
public:
  static QFuture<quint32> createBook(const Book &book);
  static QFuture<BookData> getBookById(quint32 bookId);
  static QFuture<QByteArray> updateBook(const Book &book);
  static QFuture<QByteArray> deleteBookById(quint32 bookId);
};

#endif /* !BOOKCONTROLLER_H */
