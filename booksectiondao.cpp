#include <QtConcurrent>

#include "booksectiondao.h"

const QString BookSectionDAO::kQuery = R"(
SELECT
  ba.book_id,
  ba.book_title,
  ba.categories,
  ba.authors,
  ba.cover_path
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

      QList<BookCardData> cardsData(table.size());

      QtConcurrent::blockingMap(table, [&cardsData,
                                        defaultBookCover](TableRow &row) {
        quint32 book_id = row.data[0].toUInt();
        QString title = row.data[1].toString();

        QStringList categories = row.data[2].toString().split(", ");
        QStringList authors = row.data[3].toString().split(", ");

        QPixmap cover(row.data[4].toString().isNull() ? defaultBookCover
                                                      : row.data[4].toString());

        cardsData[row.index] =
          BookCardData(cover, title, book_id, authors, categories, 5);
      });

      return cardsData;
    });
}

void BookSectionDAO::setSearchFilter(const QString &search) {
  QString condition = "lower(ba.book_title) REGEXP :search";
  QString reg = search.isEmpty()
                  ? ""
                  : search.toLower().split("", Qt::SkipEmptyParts).join(".*");

  m_orderBy = "length(ba.book_title) - " + QString::number(search.length());

  m_bindings[":search"] = reg;

  setFilter(BookSectionDAO::Search, condition);
}

QFuture<quint32> BookSectionDAO::bookCardsCount() {
  QString cmd = "SELECT count(*) FROM book_author_vw AS ba WHERE %1";
  QString filters = applyFilters();

  return LibraryDatabase::exec(cmd.arg(filters), m_bindings)
    .then([](LibraryTable table) { return table[0].data[0].toUInt(); });
}
