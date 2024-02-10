#include <QString>

#include "bookcategory.h"
#include "librarydatabase.h"

QFuture<void> BookCategory::update(quint32 book_id,
                                   QList<quint32> category_id) {
  return LibraryDatabase::remove(
           "DELETE FROM book_category WHERE book_id = :book_id",
           {{":book_id", book_id}})
    .then(QtFuture::Launch::Async, [=]() {
      QString cmd = "INSERT INTO book_category (book_id, category_id) VALUES ";

      SqlBindingHash bindings = {
        {":book_id", book_id},
      };

      for (qsizetype i = 0; i < category_id.size(); ++i) {
        QString categoryBinding = ":category_id" + QString::number(i);

        cmd += QString("(:book_id, %1),").arg(categoryBinding);
        bindings.insert(categoryBinding, category_id[i]);
      }

      LibraryDatabase::update(cmd.chopped(1), bindings).waitForFinished();
    });
}

QFuture<QList<BookCategory>> BookCategory::categories() {
  return LibraryDatabase::exec("SELECT category_id, name FROM category")
    .then(QtFuture::Launch::Async, [](const LibraryTable &table) {
      QList<BookCategory> categories;
      categories.reserve(table.size());

      for (const QVariantList &row : table) {
        quint32 categoryId = row[0].toUInt();
        QString categoryName = row[1].toString();

        categories.emplaceBack(categoryId, categoryName);
      }

      return categories;
    });
}
