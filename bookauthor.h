#ifndef BOOKAUTHOR_H
#define BOOKAUTHOR_H

#include <QFuture>
#include <QtGlobal>

class BookAuthor {
public:
  static QFuture<void> update(quint32 book_id, QList<quint32> author_id);
};

#endif  // BOOKAUTHOR_H
