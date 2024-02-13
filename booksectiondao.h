#ifndef BOOKSECTIONDAO_H
#define BOOKSECTIONDAO_H

#include "bookcard.h"
#include "librarydatabase.h"

class BookSectionDAO {
public:
  enum Filter { Search = 0, Category, _LastFilter = Category };

  enum Column { Id = 0, Title, Rating, _LastColumn = Rating };

public:
  BookSectionDAO();
  QFuture<QList<BookCardData>> loadBookCards(int itemsCount, int offset,
                                             const QPixmap &defaultBookCover);
  QFuture<quint32> bookCardsCount();

  void setSearchFilter(const QString &search);

  void setFilter(Filter filter, const QString &value = "");

  void orderBy(Column column, Qt::SortOrder order = Qt::AscendingOrder);

private:
  QString applyFilters();

  static const QString kQuery;

  SqlBindingHash m_bindings;

  QString m_filters[_LastFilter + 1];
  QString m_orderBy;
};

Q_DECLARE_METATYPE(BookSectionDAO::Column)

#endif  // BOOKSECTIONDAO_H
