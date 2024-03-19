#include <QtConcurrent>

#include <QDateTime>
#include <QSqlError>
#include <QStandardItemModel>

#include "bookadddialog.h"
#include "libraryapplication.h"
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

  m_dao = new BookSectionDAO(this);
  m_bookAddDialog = new BookAddDialog(this);
  m_searchFilterDialog = new SearchFilterDialog(m_dao, this);
  QStandardItemModel *model = new QStandardItemModel(this);

  ui->bookListView->setModel(model);
  ui->bookListView->setUniformItemSizes(true);
  ui->pagination->setItemsPerPageCount(20);

  for (int i = 0; i < ui->pagination->itemsPerPageCount(); ++i) {
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

  connect(action, &QAction::triggered, m_searchFilterDialog,
          &SearchFilterDialog::open);
  connect(m_searchFilterDialog, &QDialog::accepted, this,
          &BookSection::reloadPage);

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
  connect(ui->pagination, &Pagination::pageChanged, this,
          &BookSection::loadPage);
}

BookSection::~BookSection() {
  delete ui;
}

void BookSection::hideItems(quint32 itemStart) {
  for (int i = itemStart; i < ui->pagination->itemsPerPageCount(); ++i) {
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
  if (!m_booksCount) {
    hideItems();
    return;
  }

  qint32 accumulatedItems = pageNumber * ui->pagination->itemsPerPageCount();
  QPixmap defaultBookCover(":/images/DefaultBookCover");

  m_dao->loadBookCards(ui->pagination->itemsPerPageCount(), accumulatedItems)
    .then(this,
          [this, defaultBookCover = std::move(defaultBookCover)](
            const QList<BookCardData> &bookCards) {
            for (qsizetype item = 0; item < bookCards.size(); ++item) {
              const BookCardData &data = bookCards[item];
              QModelIndex itemIndex = ui->bookListView->model()->index(item, 0);
              BookCard *bookCard = qobject_cast<BookCard *>(
                ui->bookListView->indexWidget(itemIndex));

              ui->bookListView->setRowHidden(item, false);
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

void BookSection::loadBooks() {
  updateLastSync();

  updateNumberOfBooks().then(this, [this]() {
    reloadPage();
    distributeGridSize();
  });
}

void BookSection::synchronizeNowButtonClicked() {
  LibraryDatabase::reopen()
    .then(QtFuture::Launch::Async,
          [this]() { updateNumberOfBooks().waitForFinished(); })
    .then(this, [this]() { reloadPage(); })
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);
    });

  updateLastSync();
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

  updateNumberOfBooks().then(this, [this]() {
    ui->pagination->setPage(0);
    reloadPage();
  });
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

void BookSection::bookInsertedHandle() {
  setBooksCount(m_booksCount + 1);

  reloadPage();
}

void BookSection::setBooksCount(qint32 booksCount) {
  m_booksCount = booksCount;

  ui->pagination->setItemsCount(booksCount);
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

void BookSection::deleteButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->bookListView->selectionModel()->selectedIndexes();

  for (const QModelIndex &index : selectedIndexes) {
    BookCard *bookCard =
      qobject_cast<BookCard *>(ui->bookListView->indexWidget(index));

    LibraryDatabase::deleteById<Book>(bookCard->bookId());
  }
}

void BookSection::reloadPage() {
  loadPage(ui->pagination->currentPage());
}
