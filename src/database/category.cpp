#include <QString>

#include "bookcategory.h"
#include "category.h"
#include "database/librarydatabase.h"

QFuture<void> Category::update(quint32 book_id, QList<quint32> category_id) {
  return LibraryDatabase::deleteById<BookCategory>(book_id).then(
    QtFuture::Launch::Async, [=](auto) {
      QList<BookCategory> categories;
      for (quint32 cat_id : category_id) {
        BookCategory bc(book_id, cat_id);
        categories.push_back(bc);
      }

      LibraryDatabase::insertBatch(categories, true).waitForFinished();
    });
}
