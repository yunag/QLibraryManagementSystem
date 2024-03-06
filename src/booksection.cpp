#include <QtConcurrent>

#include <QDateTime>
#include <QMessageBox>
#include <QRegularExpressionValidator>
#include <QScrollBar>
#include <QSqlError>
#include <QStandardItemModel>

#include "bookadddialog.h"
#include "bookdetailsdialog.h"
#include "searchfilterdialog.h"

#include "bookcard.h"
#include "booksection.h"
#include "smoothscrollbar.h"

#include "database/booksectiondao.h"
#include "database/librarydatabase.h"
#include "ui_booksection.h"

BookSection::BookSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::BookSection) {
  ui->setupUi(this);

  m_currentPage = 0;

  m_dao = new BookSectionDAO(this);
  m_bookAddDialog = new BookAddDialog(this);
  m_searchFilterDialog = new SearchFilterDialog(m_dao, this);
  QStandardItemModel *model = new QStandardItemModel(this);

  ui->bookListView->setModel(model);
  ui->bookListView->setUniformItemSizes(true);

  for (int i = 0; i < kItemsPerPage; ++i) {
    QStandardItem *bookItem = new QStandardItem;
    BookCard *bookCard = new BookCard(ui->bookListView);

    bookItem->setSizeHint(bookCard->sizeHint());
    model->insertRow(i, bookItem);
    ui->bookListView->setIndexWidget(model->index(i, 0), bookCard);
  }

  SmoothScrollBar *vScrollBar = new SmoothScrollBar(ui->bookListView);
  ui->bookListView->setVerticalScrollBar(vScrollBar);
  ui->bookListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->bookListView->horizontalScrollBar()->setDisabled(true);

  ui->bookListView->setAcceptDrops(false);
  ui->bookListView->setDragDropMode(QAbstractItemView::NoDragDrop);

  ui->searchLineEdit->setClearButtonEnabled(true);
  QAction *action = ui->searchLineEdit->addAction(QIcon(":/icons/searchIcon"),
                                                  QLineEdit::LeadingPosition);

  LibraryDatabase::transaction().onFailed(
    this, [this](const QSqlError &err) { databaseErrorMessageBox(this, err); });

  connect(action, &QAction::triggered, this,
          [this]() { m_searchFilterDialog->open(); });
  connect(m_searchFilterDialog, &QDialog::accepted, this,
          &BookSection::searchFilterAccepted);

  connect(ui->bookListView, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
            BookCard *bookCard =
              qobject_cast<BookCard *>(ui->bookListView->indexWidget(index));
            emit bookDetailsRequested(bookCard->bookId());
          });

  connect(ui->deleteButton, &QPushButton::clicked, this,
          &BookSection::deleteButtonClicked);
  connect(ui->saveChangesButton, &QPushButton::clicked, this,
          &BookSection::saveChanges);
  connect(m_bookAddDialog, &BookAddDialog::inserted, this,
          &BookSection::bookInsertedHandle);
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
    ui->bookListView->setRowHidden(i, true);
  }
}

void BookSection::updateLastSync() {
  QDateTime dateTime = QDateTime::currentDateTime();
  QString formattedDate = dateTime.toString("dd/MM/yyyy hh:mm:ss");

  ui->lastSyncLabel->setText(formattedDate);
}

QFuture<void> BookSection::updateNumberOfBooks() {
  return m_dao->bookCardsCount()
    .then(this, [this](quint32 count) { setBooksCount(count); })
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);

      setBooksCount(0);
    });
}

void BookSection::loadPage(qint32 pageNumber) {
  if (!m_booksCount || !isValidPage(pageNumber)) {
    hideItems();
    return;
  }

  updatePageButtons(pageNumber);

  qint32 accumulatedItems = pageNumber * kItemsPerPage;
  QPixmap defaultBookCover(":/images/DefaultBookCover");

  m_dao->loadBookCards(kItemsPerPage, accumulatedItems)
    .then(this,
          [this, defaultBookCover = std::move(defaultBookCover)](
            const QList<BookCardData> &bookCards) {
            for (qsizetype itemNumber = 0; itemNumber < bookCards.size();
                 ++itemNumber) {
              const BookCardData &data = bookCards[itemNumber];
              QModelIndex itemIndex =
                ui->bookListView->model()->index(itemNumber, 0);
              BookCard *bookCard = qobject_cast<BookCard *>(
                ui->bookListView->indexWidget(itemIndex));

              ui->bookListView->setRowHidden(itemNumber, false);
              bookCard->setCover(data.cover.isNull() ? defaultBookCover
                                                     : data.cover);

              bookCard->setTitle(data.title);
              bookCard->setBookId(data.bookId);
              bookCard->setAuthors(data.authors);
              bookCard->setCategories(data.categories);
              bookCard->setRating(qRound(data.rating));
            }

            hideItems(bookCards.size());
          })
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);
    });
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
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);
    });

  updateLastSync();
}

void BookSection::nextPageButtonClicked() {
  if (isValidPage(m_currentPage + 1)) {
    m_currentPage += 1;
    loadPage(m_currentPage);
  }
}

void BookSection::prevPageButtonClicked() {
  if (isValidPage(m_currentPage - 1)) {
    m_currentPage -= 1;
    loadPage(m_currentPage);
  }
}

void BookSection::addButtonClicked() {
  m_bookAddDialog->open();
}

void BookSection::resizeEvent(QResizeEvent *event) {
  distributeGridSize();

  QWidget::resizeEvent(event);
}

void BookSection::searchTextChanged(const QString &text) {
  m_dao->setSearchFilter(text);
  /* Reset page to first */
  m_currentPage = 0;

  updateNumberOfBooks().then(this, [this]() { loadPage(m_currentPage); });
}

void BookSection::distributeGridSize() {
  QListView *bookList = ui->bookListView;
  QStandardItemModel *model =
    qobject_cast<QStandardItemModel *>(bookList->model());
  QStandardItem *item = model->item(0);

  if (!item) {
    return;
  }

  int itemWidth = item->sizeHint().width();
  int itemHeight = item->sizeHint().height();

  int viewportWidth =
    bookList->width() - bookList->verticalScrollBar()->width();

  int numItemsInRow = viewportWidth / itemWidth;
  if (numItemsInRow) {
    int totalItemsWidth = numItemsInRow * itemWidth;

    int remaindedWidth = viewportWidth - totalItemsWidth;
    int evenlyDistributedWidth = remaindedWidth / numItemsInRow;

    const int extraHOffset = -3;
    const int extraVSpace = 6;
    QSize newGridSize(itemWidth + evenlyDistributedWidth + extraHOffset,
                      itemHeight + extraVSpace);
    bookList->setGridSize(newGridSize);
  }
}

void BookSection::updatePageButtons(qint32 pageNumber) {
  QWidget *widget = focusWidget();

  ui->prevPageButton->setDisabled(isStartPage(pageNumber));
  ui->nextPageButton->setDisabled(isEndPage(pageNumber));

  if (widget != focusWidget()) {
    ui->bookListView->setFocus();
  }
}

void BookSection::bookInsertedHandle() {
  setBooksCount(m_booksCount + 1);

  /* reload current page */
  loadPage(m_currentPage);
}

void BookSection::setBooksCount(qint32 booksCount) {
  m_booksCount = booksCount;
  ui->numberOfBooksLabel->setText(QString::number(m_booksCount));
}

void BookSection::saveChanges() {
  ui->saveChangesButton->setEnabled(false);

  LibraryDatabase::commit()
    .then(QtFuture::Launch::Async, []() { LibraryDatabase::transaction(); })
    .then(this, [this]() { ui->saveChangesButton->setEnabled(true); })
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);
    });
}

void BookSection::searchFilterAccepted() {
  loadPage(m_currentPage);
}

void BookSection::deleteButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->bookListView->selectionModel()->selectedIndexes();

  for (QModelIndex index : selectedIndexes) {
    BookCard *bookCard =
      qobject_cast<BookCard *>(ui->bookListView->indexWidget(index));

    LibraryDatabase::deleteById<Book>(bookCard->bookId());
  }
}

bool BookSection::isValidPage(qint32 pageNumber) {
  qint32 accumulatedItems = pageNumber * kItemsPerPage;

  return pageNumber >= 0 && accumulatedItems < m_booksCount;
}
