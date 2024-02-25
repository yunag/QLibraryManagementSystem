#include "database/bookcategory.h"
#include "database/librarydatabase.h"

QFuture<void> BookCategory::update(quint32 book_id,
                                   QList<quint32> category_ids) {
  return LibraryDatabase::deleteById<BookCategory>(book_id).then(
    QtFuture::Launch::Async, [=](auto) {
      QList<BookCategory> categories;
      for (quint32 category_id : category_ids) {
        categories.push_back(BookCategory(book_id, category_id));
      }

      LibraryDatabase::insertBatch(categories, true).waitForFinished();
    });
}
