#include "bookauthor.h"
#include "librarydatabase.h"

QFuture<void> BookAuthor::update(quint32 book_id,
                                 const QList<quint32> &author_ids) {
  return LibraryDatabase::deleteById<BookAuthor>(book_id).then(
    QtFuture::Launch::Async, [=]() {
      QList<BookAuthor> bookAuthors;
      for (quint32 author_id : author_ids) {
        BookAuthor bookAuthor(book_id, author_id);
        bookAuthors.push_back(bookAuthor);
      }

      LibraryDatabase::insertBatch(bookAuthors, true);
    });
}
