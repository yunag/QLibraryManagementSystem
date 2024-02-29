#ifndef BOOKDETAILSDAO_H
#define BOOKDETAILSDAO_H

#include <QFuture>
#include <QObject>

#include "author.h"
#include "book.h"
#include "category.h"

struct BookDetails {
  QList<Author> authors;
  QList<Category> categories;

  Book book;
};

class BookDetailsDAO : public QObject {
  Q_OBJECT

public:
  explicit BookDetailsDAO(QObject *parent = nullptr);
  QFuture<BookDetails> fetchDetails(quint32 bookId);

protected:
  static const QString kQuery;
};

#endif  // BOOKDETAILSDAO_H
