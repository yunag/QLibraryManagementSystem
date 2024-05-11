#include <QNetworkReply>

#include <QJsonArray>
#include <QJsonObject>

#include "network/network.h"

#include "common/json.h"

#include "booksectiondao.h"
#include "libraryapplication.h"

using Filter = BookSectionDao::Filter;
using Property = BookSectionDao::Property;

BookSectionDao::BookSectionDao(QObject *parent)
    : QObject(parent), m_order(Qt::AscendingOrder),
      m_propertyOrder(Property::TitleProperty) {}

static QString filterToApi(Filter filter) {
  switch (filter) {
    case Filter::TitleFilter:
      return "title";
    case Filter::CategoryFilter:
      return "category";
    default:
      return {};
  }
}

static QString propertyToApi(Property property) {
  switch (property) {
    case Property::IdProperty:
      return "id";
    case Property::TitleProperty:
      return "title";
    case Property::RatingProperty:
      return "rating";
    default:
      qDebug() << "Invalid property";
      return {};
  }
}

void BookSectionDao::addFilter(Filter filter, const QString &value) {
  m_filters[filter] = value;
}

void BookSectionDao::removeFilter(Filter filter) {
  m_filters.remove(filter);
}

static QString sortToApi(Property property, Qt::SortOrder order) {
  QString api = propertyToApi(property);

  if (order == Qt::AscendingOrder) {
    api += "-asc";
  }

  return api;
}

QFuture<QList<BookData>> BookSectionDao::loadBookCards(int itemsCount,
                                                       int offset) {
  QString orderBy = sortToApi(m_propertyOrder, m_order);
  QUrlQuery query;

  for (const auto &[filter, value] : m_filters.asKeyValueRange()) {
    QString api = filterToApi(filter);

    query.addQueryItem(api, value);
  }

  query.addQueryItem("orderby", orderBy);
  query.addQueryItem("limit", QString::number(itemsCount));
  query.addQueryItem("offset", QString::number(offset));
  query.addQueryItem("includeauthors", "");
  query.addQueryItem("includecategories", "");

  RestApiManager *networkManager = App->network();

  auto [future, reply] = networkManager->get("/api/books", query);

  return future.then([](const QByteArray &data) {
    QList<BookData> booksData;

    QJsonArray books = json::byteArrayToJson(data)->array();

    for (const auto bookJson : books) {
      QJsonObject json = bookJson.toObject();

      booksData.push_back(BookData::fromJson(json));
    }
    return booksData;
  });
}

void BookSectionDao::orderBy(Property property, Qt::SortOrder order) {
  m_order = order;
  m_propertyOrder = property;
}

Qt::SortOrder BookSectionDao::order() const {
  return m_order;
}

Property BookSectionDao::orderProperty() const {
  return m_propertyOrder;
}

QFuture<int> BookSectionDao::bookCardsCount() const {
  QUrlQuery query;

  for (const auto &[filter, value] : m_filters.asKeyValueRange()) {
    QString api = filterToApi(filter);

    query.addQueryItem(api, value);
  }

  RestApiManager *manager = App->network();
  auto [future, reply] = manager->get("/api/books/count", query);

  return future.then([](const QByteArray &data) {
    QJsonDocument json = *json::byteArrayToJson(data);

    return json["count"].toInt();
  });
}
