#ifndef BOOKSECTIONDAO_H
#define BOOKSECTIONDAO_H

#include <QUrlQuery>

#include <QFuture>

#include "schema/schema.h"

class RestApiManager;

class BookSectionDao : public QObject {
public:
  enum Filter { TitleFilter = 0, CategoryFilter };
  Q_ENUM(Filter)

  enum Property { IdProperty = 0, TitleProperty, RatingProperty };
  Q_ENUM(Property)

public:
  BookSectionDao(QObject *parent = nullptr);
  QFuture<QList<BookData>> loadBookCards(int itemsCount, int offset);

  void addFilter(Filter filter, const QString &value);
  void removeFilter(Filter filter);

  void orderBy(Property property, Qt::SortOrder order = Qt::AscendingOrder);
  Qt::SortOrder order() const;
  Property orderProperty() const;

  QFuture<int> bookCardsCount() const;

private:
  QMap<Filter, QString> m_filters;

  Qt::SortOrder m_order;
  Property m_propertyOrder;
};

#endif  // BOOKSECTIONDAO_H
