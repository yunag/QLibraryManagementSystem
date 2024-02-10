#ifndef BOOKAUTHOR_H
#define BOOKAUTHOR_H

#include <QFuture>
#include <QtGlobal>

class BookAuthor {
public:
  quint32 id;
  QString firstName;
  QString lastName;

  BookAuthor(quint32 id, QString firstName, QString lastName)
      : id(id), firstName(std::move(firstName)), lastName(std::move(lastName)) {
  }

  static QFuture<void> update(quint32 book_id, const QList<quint32> &author_id);
  static QFuture<QList<BookAuthor>> authors();
};

#endif  // BOOKAUTHOR_H
