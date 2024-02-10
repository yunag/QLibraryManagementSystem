#ifndef BOOKCATEGORY_H
#define BOOKCATEGORY_H

#include <QFuture>
#include <QtGlobal>

class BookCategory {
public:
  quint32 id;
  QString name;

  BookCategory(quint32 id, QString name) : id(id), name(std::move(name)) {}

  static QFuture<void> update(quint32 book_id, QList<quint32> category_id);
  static QFuture<QList<BookCategory>> categories();
};

#endif  // BOOKCATEGORY_H
