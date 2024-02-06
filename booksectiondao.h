#ifndef BOOKSECTIONDAO_H
#define BOOKSECTIONDAO_H

#include "bookcard.h"
#include "librarydatabase.h"

class BookSectionDAO {
public:
  enum Filter { Search = 0, Category, _Last = Category };

public:
  BookSectionDAO();
  QFuture<QList<BookCardData>> loadBookCards(int itemsCount, int offset,
                                             const QPixmap &defaultBookCover);
  QFuture<quint32> bookCardsCount();

  void setSearchFilter(const QString &search);

  void setFilter(Filter filter, const QString &value = "");

private:
  QString applyFilters();

  static const QString kQuery;

  SqlBindingHash m_bindings;

  QString m_filters[_Last + 1];
  QString m_orderBy;
};

#endif  // BOOKSECTIONDAO_H
