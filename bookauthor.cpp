#include "bookauthor.h"
#include "librarydatabase.h"

QFuture<void> BookAuthor::update(quint32 book_id,
                                 const QList<quint32> &author_id) {
  return LibraryDatabase::remove(
           "DELETE FROM book_author WHERE book_id = :book_id",
           {{":book_id", book_id}})
    .then(QtFuture::Launch::Async, [=]() {
      QString cmd = "INSERT INTO book_author (book_id, author_id) VALUES ";

      SqlBindingHash bindings = {
        {":book_id", book_id},
      };

      for (qsizetype i = 0; i < author_id.size(); ++i) {
        QString authorBinding = ":author_id" + QString::number(i);

        cmd += QString("(:book_id, %1),").arg(authorBinding);
        bindings[authorBinding] = author_id[i];
      }

      LibraryDatabase::update(cmd.chopped(1), bindings).waitForFinished();
    });
}

QFuture<QList<BookAuthor>> BookAuthor::authors() {
  return LibraryDatabase::exec(
           "SELECT author_id, first_name, last_name FROM author")
    .then(QtFuture::Launch::Async, [](const LibraryTable &table) {
      QList<BookAuthor> authors;
      authors.reserve(table.size());

      for (const QVariantList &row : table) {
        quint32 authorId = row[0].toUInt();
        QString firstName = row[1].toString();
        QString lastName = row[2].toString();

        authors.emplaceBack(authorId, firstName, lastName);
      }

      return authors;
    });
}
