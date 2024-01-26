#ifndef BOOKSECTIONDAO_H
#define BOOKSECTIONDAO_H

#include <QString>
#include <QtConcurrent>

#include "bookcard.h"
#include "qlibrarydatabase.h"

class BookSectionDAO {
public:
  enum Filter { Search = 0, Category, _Last = Category };

public:
  BookSectionDAO();
  QFuture<QList<BookCardData>> loadBookCards(int nItems, int offset);
  QFuture<quint32> bookCardsCount();

  void setSearchFilter(const QString &search);

  void setFilter(Filter filter, const QString &value = "");

private:
  QString applyFilters();

  static QString kQuery;

  SqlBindingHash m_bindings;

  QString m_filters[_Last + 1];
  QString m_orderBy;
};

#endif  // BOOKSECTIONDAO_H
