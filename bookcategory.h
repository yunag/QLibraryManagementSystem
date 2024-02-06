#ifndef BOOKCATEGORY_H
#define BOOKCATEGORY_H

#include <QFuture>
#include <QtGlobal>

class BookCategory {
public:
  static QFuture<void> update(quint32 book_id, QList<quint32> category_id);
};

#endif  // BOOKCATEGORY_H
