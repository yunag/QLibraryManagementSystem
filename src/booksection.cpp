#include <QtConcurrent>

#include <QDateTime>
#include <QStandardItemModel>

#include "booksection.h"
#include "ui_booksection.h"

#include "dao/booksectiondao.h"

#include "common/error.h"
#include "common/widgetutils.h"
#include "controllers/bookcontroller.h"
#include "controllers/bookratingcontroller.h"

#include "schema/schema.h"

#include "bookadddialog.h"
#include "bookcard.h"
#include "searchfilterdialog.h"
#include "smoothscrollbar.h"

BookSection::BookSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::BookSection), m_dao(new BookSectionDao(this)),
      m_bookAddDialog(new BookAddDialog(this)),
      m_searchFilterDialog(new SearchFilterDialog(m_dao, this)) {
  ui->setupUi(this);

  auto *model = new QStandardItemModel(this);

  ui->bookListView->setModel(model);
  ui->bookListView->setUniformItemSizes(true);
  ui->pagination->setItemsPerPageCount(20);

  for (int i = 0; i < ui->pagination->itemsPerPageCount(); ++i) {
    auto *bookItem = new QStandardItem;
    auto *bookCard = new BookCard(ui->bookListView);

    connect(bookCard, &BookCard::ratingChanged, bookCard,
            [bookCard](int rating) {
              BookRatingController::rateBook(bookCard->bookId(), rating);
            });

    bookItem->setSizeHint(bookCard->sizeHint());
    model->insertRow(i, bookItem);
    ui->bookListView->setIndexWidget(model->index(i, 0), bookCard);
  }

  auto *vScrollBar = new SmoothScrollBar(ui->bookListView);
  ui->bookListView->setVerticalScrollBar(vScrollBar);
  ui->bookListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->bookListView->horizontalScrollBar()->setDisabled(true);

  ui->bookListView->setAcceptDrops(false);
  ui->bookListView->setDragDropMode(QAbstractItemView::NoDragDrop);

  ui->searchLineEdit->setClearButtonEnabled(true);
  QAction *action = ui->searchLineEdit->addAction(QIcon(":/icons/searchIcon"),
                                                  QLineEdit::LeadingPosition);

  connect(action, &QAction::triggered, m_searchFilterDialog,
          &SearchFilterDialog::open);
  connect(m_searchFilterDialog, &QDialog::accepted, this,
          &BookSection::reloadCurrentPage);

  connect(ui->bookListView, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
            auto *bookCard =
              qobject_cast<BookCard *>(ui->bookListView->indexWidget(index));
            emit bookDetailsRequested(bookCard->bookId());
          });

  connect(ui->deleteButton, &QPushButton::clicked, this,
          &BookSection::deleteButtonClicked);
  connect(ui->saveChangesButton, &QPushButton::clicked, this,
          &BookSection::saveChanges);
  connect(m_bookAddDialog, &BookAddDialog::edited, this,
          &BookSection::bookInsertedHandle);
  connect(ui->addButton, &QPushButton::clicked, this,
          &BookSection::addButtonClicked);
  connect(ui->updateButton, &QPushButton::clicked, this,
          &BookSection::updateButtonClicked);
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

void BookSection::hideItems(int itemStart) {
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
    .then(this, [this](int count) { setBooksCount(count); })
    .onFailed(this, [this]() { setBooksCount(0); });
}

void BookSection::onBookData(const QList<BookData> &bookCards) {
  int item;
  for (item = 0; item < bookCards.size(); ++item) {
    const BookData &bookData = bookCards[item];
    QModelIndex itemIndex = ui->bookListView->model()->index(item, 0);
    auto *bookCard =
      qobject_cast<BookCard *>(ui->bookListView->indexWidget(itemIndex));

    ui->bookListView->setRowHidden(item, false);

    QStringList authors;
    for (const Author &author : bookData.authors) {
      QString fullName = author.firstName + " " + author.lastName;
      authors.push_back(fullName);
    }

    QStringList categories;
    for (const Category &category : bookData.categories) {
      categories.push_back(category.name);
    }

    auto reply =
      WidgetUtils::asyncLoadImage(bookCard->coverLabel(), bookData.coverUrl);
    if (reply) {
      m_imageReplies.push_back(reply);
    }

    bookCard->setTitle(bookData.title);
    bookCard->setBookId(bookData.id);
    bookCard->setAuthors(authors);
    bookCard->setCategories(categories);
    bookCard->setRating(qRound(bookData.rating));
  }

  hideItems(item);
}

void BookSection::loadPage(qint32 pageNumber) {
  qint32 accumulatedItems = pageNumber * ui->pagination->itemsPerPageCount();

  for (auto &reply : m_imageReplies) {
    reply->abort();
  }
  m_imageReplies.clear();

  m_dao->loadBookCards(ui->pagination->itemsPerPageCount(), accumulatedItems)
    .then(this,
          [this](const QList<BookData> &bookCards) { onBookData(bookCards); })
    .onFailed(this,
              [this](const NetworkError &err) { handleError(this, err); });
}

void BookSection::loadBooks() {
  updateLastSync();

  distributeGridSize();

  updateNumberOfBooks().then(this, [this]() { reloadCurrentPage(); });
}

void BookSection::synchronizeNowButtonClicked() {
  /* TODO: synchronize with server */
  updateLastSync();
}

void BookSection::addButtonClicked() {
  m_bookAddDialog->createBook();
}

void BookSection::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);

  distributeGridSize();
}

void BookSection::searchTextChanged(const QString &text) {
  if (text.isEmpty()) {
    m_dao->removeFilter(BookSectionDao::TitleFilter);
  } else {
    m_dao->addFilter(BookSectionDao::TitleFilter, text);
  }

  updateNumberOfBooks().then(this, [this]() {
    ui->pagination->setPage(0);
    reloadCurrentPage();
  });
}

void BookSection::distributeGridSize() {
  QListView *bookList = ui->bookListView;
  auto *model = qobject_cast<QStandardItemModel *>(bookList->model());
  QStandardItem *item = model->item(0);

  if (!item) {
    return;
  }

  /* NOTE: scrollbar->width() will report wrong value  
   * which is necessary for calculating currently available viewport
   * width. See https://stackoverflow.com/questions/16515646/how-to-get-scroll-bar-real-width-in-qt
   */
  int scrollBarWidth = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);

  if (bookList->verticalScrollBar()->isVisible()) {
    scrollBarWidth = bookList->verticalScrollBar()->width();
  }

  int itemWidth = item->sizeHint().width();
  int itemHeight = item->sizeHint().height();

  int viewportWidth = bookList->width() - scrollBarWidth;

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
  updateNumberOfBooks().then([this]() { reloadCurrentPage(); });
}

void BookSection::setBooksCount(qint32 booksCount) {
  m_booksCount = booksCount;

  ui->pagination->setItemsCount(booksCount);
  ui->numberOfBooksLabel->setText(QString::number(booksCount));
}

void BookSection::saveChanges() {
  ui->saveChangesButton->setEnabled(false);
}

void BookSection::deleteButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->bookListView->selectionModel()->selectedIndexes();

  for (const QModelIndex &index : selectedIndexes) {
    auto *bookCard =
      qobject_cast<BookCard *>(ui->bookListView->indexWidget(index));

    BookController::deleteBookById(bookCard->bookId())
      .then(this, [this](auto) { return updateNumberOfBooks(); })
      .then(QtFuture::Launch::Async,
            [](QFuture<void> future) { future.waitForFinished(); })
      .then(this, [this]() { reloadCurrentPage(); })
      .onFailed(this,
                [this](const NetworkError &err) { handleError(this, err); });
  }
}

void BookSection::reloadCurrentPage() {
  loadPage(ui->pagination->currentPage());
}

void BookSection::updateButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->bookListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.isEmpty()) {
    return;
  }

  auto *bookCard =
    qobject_cast<BookCard *>(ui->bookListView->indexWidget(selectedIndexes[0]));

  m_bookAddDialog->editBook(bookCard->bookId());
}
