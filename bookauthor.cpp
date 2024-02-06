#include "bookauthor.h"
#include "librarydatabase.h"

QFuture<void> BookAuthor::update(quint32 book_id, QList<quint32> author_id) {
  return LibraryDatabase::remove(
           "DELETE FROM book_author WHERE book_id = :book_id",
           {{":book_id", book_id}})
    .then([=]() {
      QString cmd = "INSERT INTO book_author (book_id, author_id) VALUES ";

      SqlBindingHash bindings = {
        {":book_id", book_id},
      };

      for (qsizetype i = 0; i < author_id.size(); ++i) {
        QString authorBinding = ":author_id" + QString::number(i);

        cmd += QString("(:book_id, %1),").arg(authorBinding);
        bindings.insert(authorBinding, author_id[i]);
      }

      LibraryDatabase::update(cmd.chopped(1), bindings).waitForFinished();
    });
}
