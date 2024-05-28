#include <QtConcurrent>

#include <QButtonGroup>
#include <QDateTime>
#include <QMovie>

#include "booksection.h"
#include "ui_booksection.h"

#include "common/error.h"
#include "controllers/bookcontroller.h"
#include "controllers/bookratingcontroller.h"

#include "model/bookresttableproxymodel.h"

#include "bookadddialog.h"
#include "bookcarddelegate.h"
#include "searchfilterdialog.h"
#include "smoothscrollbar.h"

#include "bookrestmodel.h"

BookSection::BookSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::BookSection),
      m_model(new BookRestModel(this)),
      m_bookAddDialog(new BookAddDialog(this)),
      m_searchFilterDialog(new SearchFilterDialog(m_model, this)) {
  ui->setupUi(this);

  auto *pagination = new Pagination(this);
  pagination->setPerPage(60);

  ui->pagination->setPagination(pagination);

  m_model->setOrderBy("id");
  m_model->setPagination(pagination);

  ui->bookListView->setModel(m_model);
  ui->bookListView->setUniformItemSizes(true);
  ui->bookListView->setItemDelegate(new BookCardDelegate);
  ui->bookListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->bookListView->setMouseTracking(true);

  ui->bookCover->setAspectRatio(Qt::KeepAspectRatio);

  auto *proxyTableModel = new BookRestTableProxyModel(this);
  proxyTableModel->setSourceModel(m_model);
  ui->bookTableView->setModel(proxyTableModel);

  /* Autoexclusive buttons must be in the same QButtonGroup */
  auto *buttonGroup = new QButtonGroup(this);
  buttonGroup->addButton(ui->gridViewButton);
  buttonGroup->addButton(ui->tableViewButton);

  connect(buttonGroup, &QButtonGroup::buttonToggled, this,
          [this](QAbstractButton *button, bool checked) {
    if (!checked) {
      return;
    }

    if (button == ui->gridViewButton) {
      ui->stackedWidget->setCurrentWidget(ui->gridViewPage);
    } else {
      ui->stackedWidget->setCurrentWidget(ui->tableViewPage);
    }
  });

  /* BUG: https://bugreports.qt.io/browse/QTBUG-99476 
   * Setting scrollbar will break signals between view and scroll
   *
   * auto *vScrollBar = new SmoothScrollBar(ui->bookListView);
   * ui->bookListView->setVerticalScrollBar(vScrollBar);
   */
  ui->bookListView->verticalScrollBar()->setSingleStep(8);
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
          &BookSection::loadPage);

  connect(m_model, &BookRestModel::filtersChanged, this, [this]() {
    m_model->reset();
    m_model->pagination()->setCurrentPage(0);
    m_model->reload();
  });

  connect(m_model, &BookRestModel::dataChanged, this,
          [this](const QModelIndex &topLeft, const QModelIndex & /*topRight*/,
                 const QList<int> &roles) {
    const QModelIndex &index = topLeft;

    for (int role : roles) {
      if (role == BookRestModel::RatingRole) {
        const BookCard &bookCard = m_model->get(index.row());
        BookRatingController::rateBook(bookCard.bookId(), bookCard.rating());
      }
    }
  });

  connect(pagination, &Pagination::currentPageChanged, m_model,
          [this](int /*page*/) { loadPage(); });
  connect(pagination, &Pagination::totalCountChanged, this,
          &BookSection::setBooksCount);

  connect(ui->bookListView, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
    quint32 bookId = m_model->data(index, BookRestModel::IdRole).toUInt();
    emit bookDetailsRequested(bookId);
  });

  connect(ui->deleteButton, &QPushButton::clicked, this,
          &BookSection::deleteButtonClicked);
  connect(ui->saveChangesButton, &QPushButton::clicked, this,
          &BookSection::saveChanges);
  connect(m_bookAddDialog, &BookAddDialog::edited, this,
          &BookSection::loadPage);
  connect(ui->addButton, &QPushButton::clicked, this,
          &BookSection::addButtonClicked);
  connect(ui->updateButton, &QPushButton::clicked, this,
          &BookSection::updateButtonClicked);
  connect(ui->synchronizeNowButton, &QPushButton::clicked, this,
          &BookSection::synchronizeNowButtonClicked);
  connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
          &BookSection::searchTextChanged);
}

BookSection::~BookSection() {
  delete ui;
}

void BookSection::updateLastSync() {
  QDateTime dateTime = QDateTime::currentDateTime();
  QString formattedDate = QLocale().toString(dateTime);

  ui->lastSyncLabel->setText(formattedDate);
}

void BookSection::loadBooks() {
  updateLastSync();

  connect(m_model, &BookRestModel::reloadFinished, this,
          &BookSection::distributeGridSize, Qt::SingleShotConnection);
  m_model->reload();
}

void BookSection::synchronizeNowButtonClicked() {
  /* TODO: synchronize with the server */
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
  QVariantMap filters = m_model->filters();
  if (!text.isEmpty()) {
    filters["title"] = text;
  } else {
    filters.remove("title");
  }

  m_model->setFilters(filters);
}

void BookSection::distributeGridSize() {
  QListView *bookList = ui->bookListView;
  auto *model = qobject_cast<BookRestModel *>(bookList->model());

  QModelIndex index = model->index(0);
  if (!index.isValid()) {
    return;
  }

  QSize itemSize = model->data(index, Qt::SizeHintRole).toSize();

  /* NOTE: scrollbar->width() will report wrong value  
   * which is necessary for calculating currently available viewport
   * width. See https://stackoverflow.com/questions/16515646/how-to-get-scroll-bar-real-width-in-qt
   */
  int scrollBarWidth = qApp->style()->pixelMetric(QStyle::PM_ScrollBarExtent);

  if (bookList->verticalScrollBar()->isVisible()) {
    scrollBarWidth = bookList->verticalScrollBar()->width();
  }

  int itemWidth = itemSize.width();
  int itemHeight = itemSize.height();

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

void BookSection::setBooksCount(qint32 booksCount) {
  ui->numberOfBooksLabel->setText(QString::number(booksCount));
}

void BookSection::saveChanges() {
  ui->saveChangesButton->setEnabled(false);
}

void BookSection::deleteButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->bookListView->selectionModel()->selectedIndexes();

  for (const QModelIndex &index : selectedIndexes) {
    quint32 bookId = m_model->get(index.row()).bookId();

    BookController::deleteBookById(bookId)
      .then(this, [this, index](auto) {
      m_model->removeRow(index.row());
    }).onFailed(this, [this](const NetworkError &err) {
      handleError(this, err);
    });
  }
}

void BookSection::updateButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->bookListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.isEmpty()) {
    return;
  }

  const BookCard &bookCard = m_model->get(selectedIndexes.first().row());

  m_bookAddDialog->editBook(bookCard.bookId());
}

void BookSection::loadPage() {
  m_model->reset();
  m_model->reload();
}
