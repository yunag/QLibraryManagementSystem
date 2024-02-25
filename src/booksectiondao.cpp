#include <QtConcurrent>

#include "booksectiondao.h"

const QString BookSectionDAO::kQuery = R"(
SELECT
  bi.book_id,
  bi.book_title,
  bi.categories,
  bi.authors,
  bi.cover_path,
  bi.rating
FROM
  book_info_vw AS bi
WHERE 
  %1
ORDER BY
  %4
LIMIT
  %2
OFFSET
  %3
)";

BookSectionDAO::BookSectionDAO() {
  m_orderBy = "bi.book_id";
}

void BookSectionDAO::setFilter(Filter filter, const QString &value) {
  m_filters[filter] = value;
}

QString BookSectionDAO::applyFilters() {
  QString filters = "1";

  for (const QString &filter : m_filters) {
    if (!filter.isEmpty()) {
      filters += " AND " + filter;
    }
  }

  return filters;
}

QFuture<QList<BookCardData>> BookSectionDAO::loadBookCards(int itemsCount,
                                                           int offset) {
  QString filters = applyFilters();
  QString cmd = kQuery.arg(filters, QString::number(itemsCount),
                           QString::number(offset), m_orderBy);

  return LibraryDatabase::exec(cmd, m_bindings)
    .then(QtFuture::Launch::Async, [itemsCount](LibraryTable table) {
      Q_ASSERT(table.size() <= itemsCount);

      return QtConcurrent::blockingMapped(table, [](const QVariantList &row) {
        quint32 book_id = row[0].toUInt();
        QString title = row[1].toString();

        QStringList categories = row[2].toString().split(", ");
        QStringList authors = row[3].toString().split(", ");

        QPixmap cover(row[4].toString());
        qreal rating = row[5].toDouble();

        return BookCardData(cover, title, book_id, authors, categories, rating);
      });
    });
}

void BookSectionDAO::setSearchFilter(const QString &search) {
  QString condition = "lower(bi.book_title) LIKE :search";
  QString reg = search.toLower().split("").join("%").replace("%%%", "%\\%%");

  m_orderBy =
    search.isEmpty() ? "bi.book_id" : "length(bi.book_title) - :length";

  m_bindings[":search"] = reg;
  m_bindings[":length"] = search.length();

  setFilter(BookSectionDAO::Search, condition);
}

QFuture<quint32> BookSectionDAO::bookCardsCount() {
  QString cmd = "SELECT count(*) FROM book_info_vw AS bi WHERE %1";
  QString filters = applyFilters();

  return LibraryDatabase::exec(cmd.arg(filters), m_bindings)
    .then([](LibraryTable table) { return table[0][0].toUInt(); });
}

static QString toDbColumn(BookSectionDAO::Column column) {
  switch (column) {
    case BookSectionDAO::Id:
      return "bi.book_id";
    case BookSectionDAO::Title:
      return "bi.book_title";
    case BookSectionDAO::Rating:
      return "bi.rating";
    default:
      qDebug() << "Invalid column";
  }
}

void BookSectionDAO::orderBy(Column column, Qt::SortOrder order) {
  QString orderStr = order == Qt::AscendingOrder ? "ASC" : "DESC";

  m_orderBy = toDbColumn(column) + " " + orderStr;

  m_order = order;
  m_column = column;
}

Qt::SortOrder BookSectionDAO::order() const {
  return m_order;
}

BookSectionDAO::Column BookSectionDAO::orderColumn() const {
  return m_column;
}
