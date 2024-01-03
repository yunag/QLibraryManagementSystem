#include <QDateTime>
#include <QListWidgetItem>
#include <QScrollBar>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QRandomGenerator>
#include <QtConcurrent/QtConcurrent>
#include <qtconcurrentrun.h>

#include "bookcard.h"
#include "booksection.h"
#include "ui_booksection.h"

BookSection::BookSection(QLibraryDatabase &db, QWidget *parent)
    : QWidget(parent), m_database(db), ui(new Ui::BookSection) {
  ui->setupUi(this);

  m_pageLoading = false;
  m_currentPage = 0;

  QListWidget *bookList = ui->booksListWidget;

  for (int i = 0; i < itemsPerPage; ++i) {
    QListWidgetItem *bookItem = new QListWidgetItem(bookList);
    BookCard *bookCard = new BookCard(bookList);

    bookItem->setSizeHint(bookCard->sizeHint());

    bookList->addItem(bookItem);
    bookList->setItemWidget(bookItem, bookCard);

    m_bookCards[i] = bookCard;
    m_bookItems[i] = bookItem;
  }

  ui->booksListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  ui->booksListWidget->verticalScrollBar()->setSingleStep(8);
  ui->booksListWidget->setAcceptDrops(false);

  ui->searchLineEdit->setClearButtonEnabled(true);

  connect(this, &BookSection::synchronized, this,
          &BookSection::synchronizedHandle, Qt::QueuedConnection);
  connect(this, &BookSection::itemProcessed, this,
          &BookSection::itemProcessedHandler, Qt::QueuedConnection);
  connect(this, &BookSection::pageDone, this, &BookSection::pageDoneHandler,
          Qt::QueuedConnection);

  connect(ui->synchronizeNowButton, &QPushButton::clicked, this,
          &BookSection::synchronizeNowButtonClicked);
  connect(ui->searchLineEdit, &QLineEdit::returnPressed, this,
          &BookSection::searchTextReturnPressed);
  connect(ui->nextPageButton, &QPushButton::clicked, this,
          &BookSection::nextPageButtonClicked);
  connect(ui->prevPageButton, &QPushButton::clicked, this,
          &BookSection::prevPageButtonClicked);
}

BookSection::~BookSection() { delete ui; }

void BookSection::itemProcessedHandler(quint32 itemNumber, const QPixmap &cover,
                                       quint32 book_id, const QString &title,
                                       const QString &category,
                                       const QString &author) {
  QListWidgetItem *bookItem = m_bookItems[itemNumber];
  BookCard *bookCard = m_bookCards[itemNumber];

  bookItem->setHidden(false);
  bookCard->setCover(cover);

  bookCard->setTitle(title);
  bookCard->setBookId(book_id);
  bookCard->setAuthor(author);
  bookCard->setCategory(category);
}

void BookSection::hideItems(quint32 itemStart) {
  for (quint32 i = itemStart; i < itemsPerPage; ++i) {
    m_bookItems[i]->setHidden(true);
  }
}

void BookSection::pageDoneHandler(quint32 itemNumber) {
  hideItems(itemNumber);

  m_pageLoading = false;
}

void BookSection::updateLastSync() {
  QDateTime dateTime = QDateTime::currentDateTime();
  QString formattedDate = dateTime.toString("dd/MM/yyyy hh:mm:ss");

  ui->lastSyncLabel->setText(formattedDate);
}

void BookSection::setCondition(Condition cond, const QString &condStr) {
  m_conditions[cond] = condStr;
}

QString BookSection::applyConditions() {
  QString conditions;

  for (const QString &cond : m_conditions) {
    if (cond.isEmpty()) {
      continue;
    }

    if (conditions.isEmpty()) {
      conditions = "WHERE ";
    } else {
      conditions += " AND ";
    }
    conditions += cond;
  }

  return conditions;
}

void BookSection::updateNumberOfBooks() {
  QString cmd =
      "SELECT count(*) AS number_of_books FROM book_author_vw AS ba %1";
  QString conditions = applyConditions();

  QPromise<quint32> promise;

  QLibraryTable table = m_database.exec(cmd.arg(conditions)).result();

  if (table.size()) {
    m_booksCount = table[0].data[0].toUInt();
  }
  ui->numberOfBooksLabel->setText(QString::number(m_booksCount));
}

bool BookSection::loadPage(qint32 pageNumber) {
  if (m_booksCount == 0) {
    hideItems();
    return false;
  }

  if (m_pageLoading || pageNumber * itemsPerPage >= m_booksCount ||
      pageNumber < 0) {
    return false;
  }

  ui->prevPageButton->setDisabled(isStartPage(pageNumber));
  ui->nextPageButton->setDisabled(isEndPage(pageNumber));

  QString limit = QString::number(itemsPerPage);
  QString offset = QString::number(pageNumber * itemsPerPage);
  QString conditions = applyConditions();

  /* FIXME: Only for testing purposes. Remove it later */
  static const QList<QString> paths = {
      ":/resources/images/MasterAndMargaritaCover.jpg",
      ":/resources/images/MartinIdenCover.jpg",
      ":/resources/images/LapaVButylkeCover.jpg",
      ":/resources/images/Metro2033Cover.jpg",
      ":/resources/images/GoreOtUmaCover.jpg",
  };

  const QString cmd = R"(
    SELECT ba.book_id, ba.book_title, ba.categories, ba.authors
    FROM book_author_vw AS ba
    %1
    LIMIT %2 OFFSET %3
  )";

  m_pageLoading = true;

  m_database.exec(cmd.arg(conditions, limit, offset))
      .then([this](QLibraryTable table) {
        Q_ASSERT(table.size() <= itemsPerPage);

        QSharedPointer<QLibraryTable> ptable(
            new QLibraryTable(std::move(table)));

        QtConcurrent::map(*ptable, [this, ptable](QTableRow &row) {
          int randPathIndex =
              QRandomGenerator::global()->bounded(0, paths.size());
          QPixmap cover(paths[randPathIndex]);

          quint32 book_id = row.data[0].toUInt();
          QString title = row.data[1].toString();
          QString category = row.data[2].toString();
          QString author = row.data[3].toString();

          emit itemProcessed(row.index, cover, book_id, title, category,
                             author);
        }).then([this, ptable]() { emit pageDone(ptable->size()); });
      });

  /* FIX: Move cursor will sometimes select text in Book Card. This line hides
   * this */
  setFocus();
  return true;
}

bool BookSection::isEndPage(qint32 pageNumber) {
  return (pageNumber + 1) * itemsPerPage >= m_booksCount;
}

bool BookSection::isStartPage(qint32 pageNumber) { return pageNumber == 0; }

void BookSection::loadBooks() {
  updateNumberOfBooks();
  updateLastSync();

  loadPage(m_currentPage);
  distributeGridSize();
}

void BookSection::synchronizeNowButtonClicked() {
  m_database.reopen().then([this](bool ok) {
    if (ok) {
      emit synchronized();
    }
  });

  updateLastSync();
}

void BookSection::synchronizedHandle() {
  updateNumberOfBooks();
  loadPage(m_currentPage);
}

void BookSection::nextPageButtonClicked() {
  if (loadPage(m_currentPage + 1)) {
    m_currentPage += 1;
  }
}

void BookSection::prevPageButtonClicked() {
  if (loadPage(m_currentPage - 1)) {
    m_currentPage -= 1;
  }
}

void BookSection::resizeEvent(QResizeEvent *event) {
  distributeGridSize();

  QWidget::resizeEvent(event);
}

void BookSection::searchTextReturnPressed() {
  QString condition = "lower(ba.book_title) LIKE %1";
  QString queryString = "'%" + ui->searchLineEdit->text() + "%'";

  m_currentPage = 0;
  setCondition(BookSection::Search, condition.arg(queryString));

  updateNumberOfBooks();
  loadPage(m_currentPage);
}

void BookSection::distributeGridSize() {
  QListWidget *bookList = ui->booksListWidget;
  Q_ASSERT(bookList->count() > 0);

  QWidget *item = bookList->itemWidget(bookList->item(0));

  int itemWidth = item->sizeHint().width();
  int itemHeight = item->sizeHint().height();

  int viewportWidth =
      bookList->width() - bookList->verticalScrollBar()->width();

  int numItemsInRow = viewportWidth / itemWidth;
  if (numItemsInRow) {
    int totalItemsWidth = numItemsInRow * itemWidth;

    int remaindedWidth = viewportWidth - totalItemsWidth;
    int evenlyDistributedWidth = remaindedWidth / numItemsInRow;

    const int extraHOffset = 5;
    const int extraVSpace = 6;
    QSize newGridSize(itemWidth + evenlyDistributedWidth - extraHOffset,
                      itemHeight + extraVSpace);
    bookList->setGridSize(newGridSize);
  }
}
