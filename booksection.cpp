#include <QtConcurrent>

#include <QDateTime>
#include <QListWidgetItem>
#include <QMenu>
#include <QScrollBar>

#include "bookcard.h"
#include "booksection.h"
#include "qlibrarydatabase.h"
#include "ui_booksection.h"

BookSection::BookSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::BookSection) {
  ui->setupUi(this);

  m_pageLoading = false;
  m_currentPage = 0;

  QListWidget *bookList = ui->booksListWidget;

  for (int i = 0; i < kItemsPerPage; ++i) {
    QListWidgetItem *bookItem = new QListWidgetItem(bookList);
    BookCard *bookCard = new BookCard(bookList);

    bookItem->setSizeHint(bookCard->sizeHint());

    bookList->addItem(bookItem);
    bookList->setItemWidget(bookItem, bookCard);

    m_bookCards[i] = bookCard;
    m_bookItems[i] = bookItem;
  }

  m_bookAddDialog = new BookAddDialog(this);
  m_bookAddDialog->setWindowFlag(Qt::Window);

  ui->booksListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  ui->booksListWidget->verticalScrollBar()->setSingleStep(8);
  ui->booksListWidget->setAcceptDrops(false);

  ui->searchLineEdit->setClearButtonEnabled(true);
  QAction *action = ui->searchLineEdit->addAction(
    QIcon(":/resources/images/searchIcon.png"), QLineEdit::LeadingPosition);

  connect(action, &QAction::triggered, this,
          [action](bool checked) { qDebug() << "Triggered"; });

  connect(ui->addButton, &QPushButton::clicked, this,
          &BookSection::addButtonClicked);
  connect(ui->synchronizeNowButton, &QPushButton::clicked, this,
          &BookSection::synchronizeNowButtonClicked);
  connect(ui->searchLineEdit, &QLineEdit::returnPressed, this,
          &BookSection::searchTextReturnPressed);
  connect(ui->nextPageButton, &QPushButton::clicked, this,
          &BookSection::nextPageButtonClicked);
  connect(ui->prevPageButton, &QPushButton::clicked, this,
          &BookSection::prevPageButtonClicked);
}

BookSection::~BookSection() {
  delete ui;
}

void BookSection::hideItems(quint32 itemStart) {
  for (quint32 i = itemStart; i < kItemsPerPage; ++i) {
    m_bookItems[i]->setHidden(true);
  }
}

void BookSection::updateLastSync() {
  QDateTime dateTime = QDateTime::currentDateTime();
  QString formattedDate = dateTime.toString("dd/MM/yyyy hh:mm:ss");

  ui->lastSyncLabel->setText(formattedDate);
}

QFuture<void> BookSection::updateNumberOfBooks() {
  return m_DAO.bookCardsCount()
    .then(this,
          [this](quint32 count) {
            ui->numberOfBooksLabel->setText(QString::number(count));
            m_booksCount = count;
          })
    .onFailed([this](const QSqlError &error) {
      qWarning() << error.text();

      m_booksCount = 0;
      ui->numberOfBooksLabel->clear();
    });
}

bool BookSection::loadPage(qint32 pageNumber) {
  if (!m_booksCount) {
    hideItems();
    return false;
  }

  if (m_pageLoading || pageNumber * kItemsPerPage >= m_booksCount ||
      pageNumber < 0) {
    return false;
  }

  ui->prevPageButton->setDisabled(isStartPage(pageNumber));
  ui->nextPageButton->setDisabled(isEndPage(pageNumber));

  m_pageLoading = true;

  m_DAO.loadBookCards(kItemsPerPage, pageNumber * kItemsPerPage)
    .then(this,
          [this](QList<BookCardData> bookCards) {
            int itemNumber;

            for (itemNumber = 0; itemNumber < bookCards.size(); ++itemNumber) {
              BookCardData *data = &bookCards[itemNumber];
              QListWidgetItem *bookItem = m_bookItems[itemNumber];
              BookCard *bookCard = m_bookCards[itemNumber];

              bookItem->setHidden(false);
              bookCard->setCover(data->cover);

              bookCard->setTitle(data->title);
              bookCard->setBookId(data->bookId);
              bookCard->setAuthors(data->authors);
              bookCard->setCategories(data->categories);
            }

            hideItems(itemNumber);
          })
    .onFailed([](const QSqlError &error) { qWarning() << error.text(); })
    .then([this]() { m_pageLoading = false; });

  setFocus();
  return true;
}

bool BookSection::isEndPage(qint32 pageNumber) {
  return (pageNumber + 1) * kItemsPerPage >= m_booksCount;
}

bool BookSection::isStartPage(qint32 pageNumber) {
  return pageNumber == 0;
}

void BookSection::loadBooks() {
  updateNumberOfBooks().then(this, [this]() {
    updateLastSync();

    loadPage(m_currentPage);
    distributeGridSize();
  });
}

void BookSection::synchronizeNowButtonClicked() {
  LibraryDatabase::reopen()
    .then(QtFuture::Launch::Async,
          [this]() { updateNumberOfBooks().waitForFinished(); })
    .then(this, [this]() { loadPage(m_currentPage); })
    .onFailed([](const QSqlError &error) {
      qWarning() << "Synchronize Error:" << error.text();
    });

  updateLastSync();
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

void BookSection::addButtonClicked() {
  m_bookAddDialog->show();
}

void BookSection::resizeEvent(QResizeEvent *event) {
  distributeGridSize();

  QWidget::resizeEvent(event);
}

void BookSection::searchTextReturnPressed() {
  m_DAO.setSearchFilter(ui->searchLineEdit->text());
  /* Reset page to first */
  m_currentPage = 0;

  updateNumberOfBooks();
  loadPage(m_currentPage);
}

void BookSection::distributeGridSize() {
  QListWidget *bookList = ui->booksListWidget;
  QListWidgetItem *item = bookList->item(0);

  Q_ASSERT(item != nullptr);

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
