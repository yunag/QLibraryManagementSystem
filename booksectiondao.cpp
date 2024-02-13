#include <QtConcurrent>

#include "booksectiondao.h"

const QString BookSectionDAO::kQuery = R"(
SELECT
  ba.book_id,
  ba.book_title,
  ba.categories,
  ba.authors,
  ba.cover_path,
  ba.rating
FROM
  book_author_vw AS ba
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
  m_orderBy = "ba.book_id";
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

QFuture<QList<BookCardData>>
BookSectionDAO::loadBookCards(int itemsCount, int offset,
                              const QPixmap &defaultBookCover) {
  QString filters = applyFilters();
  QString cmd = kQuery.arg(filters, QString::number(itemsCount),
                           QString::number(offset), m_orderBy);

  return LibraryDatabase::exec(cmd, m_bindings)
    .then(QtFuture::Launch::Async, [itemsCount,
                                    defaultBookCover](LibraryTable table) {
      Q_ASSERT(table.size() <= itemsCount);

      return QtConcurrent::blockingMapped(table, [defaultBookCover](
                                                   const QVariantList &row) {
        quint32 book_id = row[0].toUInt();
        QString title = row[1].toString();

        QStringList categories = row[2].toString().split(", ");
        QStringList authors = row[3].toString().split(", ");

        QPixmap cover(row[4].toString().isNull() ? defaultBookCover
                                                 : row[4].toString());
        qreal rating = row[5].toDouble();

        return BookCardData(cover, title, book_id, authors, categories, rating);
      });
    });
}

void BookSectionDAO::setSearchFilter(const QString &search) {
  QString condition = "lower(ba.book_title) LIKE :search";
  QString reg = search.toLower().split("").join("%").replace("%%%", "%\\%%");

  m_orderBy =
    search.isEmpty() ? "ba.book_id" : "length(ba.book_title) - :length";

  m_bindings[":search"] = reg;
  m_bindings[":length"] = search.length();

  setFilter(BookSectionDAO::Search, condition);
}

QFuture<quint32> BookSectionDAO::bookCardsCount() {
  QString cmd = "SELECT count(*) FROM book_author_vw AS ba WHERE %1";
  QString filters = applyFilters();

  return LibraryDatabase::exec(cmd.arg(filters), m_bindings)
    .then([](LibraryTable table) { return table[0][0].toUInt(); });
}

void BookSectionDAO::orderBy(Column column, Qt::SortOrder order) {
  QString orderStr = order == Qt::AscendingOrder ? "ASC" : "DESC";

  switch (column) {
    case Id:
      m_orderBy = "ba.book_id";
      break;
    case Title:
      m_orderBy = "ba.book_title";
      break;
    case Rating:
      m_orderBy = "ba.rating";
      break;
    default:
      qDebug() << "Invalid column";
  }
  m_orderBy += " " + orderStr;
}
