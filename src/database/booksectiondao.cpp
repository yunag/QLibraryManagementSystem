#include <QtConcurrent>

#include "booksectiondao.h"

const QString BookSectionDAO::kQuery = R"(
SELECT
  b.book_id,
  b.title,
  (
    SELECT
      group_concat(c.name SEPARATOR ', ')
    FROM
      category c
      INNER JOIN book_category bc ON bc.category_id = c.category_id
    WHERE
      bc.book_id = b.book_id
  ) AS categories,
  (
    SELECT
      group_concat(
        concat(a.first_name, ' ', a.last_name) SEPARATOR ', '
      )
    FROM
      author a
      INNER JOIN book_author ba ON ba.author_id = a.author_id
    WHERE
      ba.book_id = b.book_id
  ) AS authors,
  b.cover_path,
  (
    SELECT
      avg(br.rating)
    FROM
      user u
      INNER JOIN book_rating br ON br.user_id = u.user_id
    WHERE
      br.book_id = b.book_id
  ) AS rating
FROM
  book b
WHERE 
  %1
ORDER BY
  %4
LIMIT
  %2
OFFSET
  %3
)";

BookSectionDAO::BookSectionDAO(QObject *parent)
    : QObject(parent), m_filters(_LastFilter + 1) {
  m_order = Qt::AscendingOrder;
  m_columnOrder = Id;
  m_filters.push_back("1");
}

void BookSectionDAO::setFilter(Filter filter, const QString &value) {
  m_filters[filter] = value;
}

QString BookSectionDAO::applyFilters() {
  return m_filters.filter(QRegularExpression("^.+$")).join(" AND ");
}

static QString columnToDbColumn(BookSectionDAO::Column column) {
  switch (column) {
    case BookSectionDAO::Id:
      return "book_id";
    case BookSectionDAO::Title:
      return "title";
    case BookSectionDAO::Rating:
      return "rating";
    default:
      qDebug() << "Invalid column";
      return "";
  }
}

static QString sortOrderToDbOrder(Qt::SortOrder order) {
  return order == Qt::AscendingOrder ? "ASC" : "DESC";
}

QFuture<QList<BookCardData>> BookSectionDAO::loadBookCards(int itemsCount,
                                                           int offset) {
  QString filters = applyFilters();
  QString orderBy =
    columnToDbColumn(m_columnOrder) + " " + sortOrderToDbOrder(m_order);
  QString cmd = kQuery.arg(filters, QString::number(itemsCount),
                           QString::number(offset), orderBy);

  return LibraryDatabase::exec(cmd, m_bindings)
    .then(QtFuture::Launch::Async, [itemsCount](LibraryTable table) {
      Q_ASSERT(table.size() <= itemsCount);

      return QtConcurrent::blockingMapped(table, [](const QVariantList &row) {
        quint32 book_id = row[0].toUInt();
        QString title = row[1].toString();

        QStringList categories = row[2].toString().split(", ");
        QStringList authors = row[3].toString().split(", ");

        QString coverPath = row[4].toString();
        QPixmap cover(coverPath);
        qreal rating = row[5].toDouble();

        return BookCardData(cover, coverPath, title, book_id, authors,
                            categories, rating);
      });
    });
}

void BookSectionDAO::setSearchFilter(const QString &search) {
  QString condition = "lower(title) LIKE :search";
  QString reg = search.toLower().split("").join("%").replace("%%%", "%\\%%");

  m_bindings[":search"] = reg;

  setFilter(BookSectionDAO::Search, condition);
}

QFuture<quint32> BookSectionDAO::bookCardsCount() {
  QString cmd = "SELECT count(*) FROM book AS b WHERE %1";
  QString filters = applyFilters();

  return LibraryDatabase::exec(cmd.arg(filters), m_bindings)
    .then([](LibraryTable table) { return table[0][0].toUInt(); });
}

void BookSectionDAO::orderBy(Column column, Qt::SortOrder order) {
  m_order = order;
  m_columnOrder = column;
}

Qt::SortOrder BookSectionDAO::order() const {
  return m_order;
}

BookSectionDAO::Column BookSectionDAO::orderColumn() const {
  return m_columnOrder;
}
