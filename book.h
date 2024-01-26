#ifndef BOOK_H
#define BOOK_H

#include <QFuture>
#include <QObject>

struct Book {
  quint32 book_id;
  QString title;
  QString publication_date;
  quint32 copies_owned;
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
  static QFuture<int> insert(const Book &book);
  static QFuture<void> remove(const Book &book);
  static QFuture<void> update(const Book &book);
};

#endif  // BOOK_H
