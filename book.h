#ifndef BOOK_H
#define BOOK_H

#include <QFuture>
#include <QObject>

struct Book {

  Book(QString title, QString publication_date, QString cover_path,
       quint32 copies_owned, quint32 book_id = 0)
      : title(std::move(title)), publication_date(std::move(publication_date)),
        cover_path(std::move(cover_path)), copies_owned(copies_owned),
        book_id(book_id) {}

  QString title;
  QString publication_date;
  QString cover_path;
  quint32 copies_owned;

  QList<quint32> author_id;
  QList<quint32> category_id;

  quint32 book_id;
};

class BookTable {
public:
  BookTable() = delete;

  ~BookTable(){};

  /**
   * @brief Insert book into `book` table
   *
   * @param book Book to insert
   * @return Book id
   */
  static QFuture<quint32> insert(const Book &book);
  static QFuture<void> remove(quint32 book_id);
  static QFuture<void> update(const Book &book);
};

#endif  // BOOK_H
