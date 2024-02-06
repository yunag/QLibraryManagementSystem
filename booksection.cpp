#include <QtConcurrent>

#include <QDateTime>
#include <QScrollBar>
#include <QSqlError>

#include "bookadddialog.h"
#include "booksectiondao.h"

#include "bookcard.h"
#include "booksection.h"
#include "librarydatabase.h"
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
  }

  m_bookAddDialog = new BookAddDialog(this);
  m_bookAddDialog->setWindowFlag(Qt::Window);

  ui->booksListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
  ui->booksListWidget->verticalScrollBar()->setSingleStep(8);
  ui->booksListWidget->setAcceptDrops(false);

  ui->searchLineEdit->setClearButtonEnabled(true);
  QAction *action = ui->searchLineEdit->addAction(
    QIcon(":/resources/images/searchIcon.png"), QLineEdit::LeadingPosition);

  m_dao = new BookSectionDAO;

  connect(action, &QAction::triggered, this, [action](bool checked) {
    Q_UNUSED(action);
    Q_UNUSED(checked);

    qDebug() << "Triggered";
  });

  connect(ui->addButton, &QPushButton::clicked, this,
          &BookSection::addButtonClicked);
  connect(ui->synchronizeNowButton, &QPushButton::clicked, this,
          &BookSection::synchronizeNowButtonClicked);
  connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
          &BookSection::searchTextChanged);
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
    ui->booksListWidget->item(i)->setHidden(true);
  }
}

void BookSection::updateLastSync() {
  QDateTime dateTime = QDateTime::currentDateTime();
  QString formattedDate = dateTime.toString("dd/MM/yyyy hh:mm:ss");

  ui->lastSyncLabel->setText(formattedDate);
}

QFuture<void> BookSection::updateNumberOfBooks() {
  return m_dao->bookCardsCount()
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

  qint32 offset = pageNumber * kItemsPerPage;

  if (m_pageLoading || offset >= m_booksCount || pageNumber < 0) {
    return false;
  }

  ui->prevPageButton->setDisabled(isStartPage(pageNumber));
  ui->nextPageButton->setDisabled(isEndPage(pageNumber));

  m_pageLoading = true;

  QPixmap defaultBookCover(":/resources/images/DefaultBookCover.jpg");

  m_dao->loadBookCards(kItemsPerPage, offset, defaultBookCover)
    .then(
      this,
      [this](QList<BookCardData> bookCards) {
        for (int itemNumber = 0; itemNumber < bookCards.size(); ++itemNumber) {
          BookCardData *data = &bookCards[itemNumber];
          QListWidgetItem *bookItem = ui->booksListWidget->item(itemNumber);
          BookCard *bookCard =
            qobject_cast<BookCard *>(ui->booksListWidget->itemWidget(bookItem));

          bookItem->setHidden(false);
          bookCard->setCover(data->cover);

          bookCard->setTitle(data->title);
          bookCard->setBookId(data->bookId);
          bookCard->setAuthors(data->authors);
          bookCard->setCategories(data->categories);
        }

        hideItems(bookCards.size());
      })
    .onFailed([](const QSqlError &error) { qWarning() << error.text(); })
    .then([this]() { m_pageLoading = false; });

  return true;
}

bool BookSection::isEndPage(qint32 pageNumber) {
  return (pageNumber + 1) * kItemsPerPage >= m_booksCount;
}

bool BookSection::isStartPage(qint32 pageNumber) {
  return pageNumber == 0;
}

void BookSection::loadBooks() {
  updateLastSync();

  updateNumberOfBooks().then(this, [this]() {
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
  m_bookAddDialog->exec();
}

void BookSection::resizeEvent(QResizeEvent *event) {
  distributeGridSize();

  QWidget::resizeEvent(event);
}

void BookSection::searchTextChanged(const QString &text) {
  m_dao->setSearchFilter(text);
  /* Reset page to first */
  m_currentPage = 0;

  updateNumberOfBooks().then([this]() { loadPage(m_currentPage); });
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
