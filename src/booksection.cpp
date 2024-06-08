#include <QtConcurrent>

#include <QButtonGroup>
#include <QScrollBar>

#include "booksection.h"
#include "ui_booksection.h"

#include "common/error.h"
#include "common/widgetutils.h"

#include "controllers/bookcontroller.h"
#include "controllers/bookratingcontroller.h"

#include "model/bookrestmodel.h"

#include "delegate/bookcarddelegate.h"
#include "delegate/tableratingdelegate.h"

#include "bookadddialog.h"
#include "booksearchfilterdialog.h"

BookSection::BookSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::BookSection),
      m_model(new BookRestModel(this)),
      m_bookAddDialog(new BookAddDialog(this)),
      m_searchFilterDialog(new BookSearchFilterDialog(m_model, this)) {
  ui->setupUi(this);

  m_loadPageTimer.setInterval(0);
  m_loadPageTimer.setSingleShot(true);

  QSettings settings;
  m_searchTimer.setInterval(
    settings.value("general/msSearchTimerUpdateSpeed").toInt());
  m_searchTimer.setSingleShot(true);

  connect(&m_loadPageTimer, &QTimer::timeout, this, [this]() {
    m_model->reset();
    m_model->reload();
  });

  connect(&m_searchTimer, &QTimer::timeout, this,
          &BookSection::searchTextChanged);

  auto *pagination = new Pagination(this);
  pagination->setPerPage(60);

  ui->pagination->setPagination(pagination);

  m_model->setOrderBy("id");
  m_model->shouldFetchImages(true);
  m_model->setPagination(pagination);

  ui->bookListView->setItemSize(BookCard::sizeHint());
  ui->bookListView->setModel(m_model);
  ui->bookListView->setItemDelegate(new BookCardDelegate);

  ui->bookCover->setAspectRatio(Qt::KeepAspectRatio);

  ui->bookTableView->setModel(m_model);
  ui->bookTableView->setSelectionModel(ui->bookListView->selectionModel());
  ui->bookTableView->setItemDelegateForColumn(BookRestModel::RatingHeader,
                                              new TableRatingDelegate);

  connect(ui->bookTableView->selectionModel(),
          &QItemSelectionModel::currentChanged, this,
          &BookSection::currentChanged);

  QHeaderView *horizontalHeader = ui->bookTableView->horizontalHeader();
  horizontalHeader->setSectionResizeMode(BookRestModel::TitleHeader,
                                         QHeaderView::Stretch);
  horizontalHeader->setSectionResizeMode(BookRestModel::AuthorsHeader,
                                         QHeaderView::Stretch);
  horizontalHeader->setSectionResizeMode(BookRestModel::CategoriesHeader,
                                         QHeaderView::Stretch);

  /* Autoexclusive buttons must be in the same QButtonGroup */
  auto *buttonGroup = new QButtonGroup(this);
  buttonGroup->addButton(ui->gridViewButton);
  buttonGroup->addButton(ui->tableViewButton);

  connect(buttonGroup, &QButtonGroup::buttonToggled, this,
          &BookSection::viewChangeButtonToggled);

  /* BUG: https://bugreports.qt.io/browse/QTBUG-99476 
   * Setting scrollbar will break signals between view and scroll
   *
   * auto *vScrollBar = new SmoothScrollBar(ui->bookListView);
   * ui->bookListView->setVerticalScrollBar(vScrollBar);
   */
  ui->bookListView->verticalScrollBar()->setSingleStep(8);
  ui->bookListView->horizontalScrollBar()->setDisabled(true);

  ui->bookListView->setAcceptDrops(false);
  ui->bookListView->setDragDropMode(QAbstractItemView::NoDragDrop);

  ui->splitter->restoreState(
    settings.value("booksection/splitter").toByteArray());

  QAction *action = ui->searchLineEdit->addAction(QIcon(":/icons/searchIcon"),
                                                  QLineEdit::LeadingPosition);

  connect(action, &QAction::triggered, m_searchFilterDialog,
          &BookSearchFilterDialog::open);

  auto handleSortFilterChange = [this]() {
    m_model->pagination()->setCurrentPage(0);
    reloadPage();
  };
  connect(m_model, &BookRestModel::orderByChanged, this,
          handleSortFilterChange);
  connect(m_model, &BookRestModel::filtersChanged, this,
          handleSortFilterChange);

  connect(m_model, &BookRestModel::dataChanged, this, &BookSection::bookRated);

  connect(pagination, &Pagination::currentPageChanged, m_model,
          [this](int page) { reloadPage(); });
  connect(pagination, &Pagination::totalCountChanged, this,
          &BookSection::setBooksCount);

  ui->showDetailsButton->setEnabled(false);
  connect(ui->showDetailsButton, &QPushButton::clicked, this,
          &BookSection::showDetailsButtonClicked);
  connect(ui->bookListView, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
    quint32 bookId = m_model->data(index, BookRestModel::IdRole).toUInt();
    emit bookDetailsRequested(bookId);
  });

  connect(ui->deleteButton, &QPushButton::clicked, this,
          &BookSection::deleteButtonClicked);
  connect(m_bookAddDialog, &BookAddDialog::edited, this,
          &BookSection::reloadPage);
  connect(ui->addButton, &QPushButton::clicked, this,
          &BookSection::addButtonClicked);
  connect(ui->updateButton, &QPushButton::clicked, this,
          &BookSection::updateButtonClicked);
  connect(ui->synchronizeNowButton, &QPushButton::clicked, this,
          &BookSection::synchronizeNowButtonClicked);
  connect(ui->searchLineEdit, &QLineEdit::textChanged, &m_searchTimer,
          [this](auto) { m_searchTimer.start(); });
}

BookSection::~BookSection() {
  QSettings settings;
  settings.setValue("booksection/splitter", ui->splitter->saveState());
  delete ui;
}

void BookSection::updateLastSync() {
  QDateTime dateTime = QDateTime::currentDateTime();
  QString formattedDate = QLocale().toString(dateTime);

  ui->lastSyncLabel->setText(formattedDate);
}

void BookSection::loadBooks() {
  updateLastSync();

  reloadPage();
}

void BookSection::synchronizeNowButtonClicked() {
  reloadPage();
  updateLastSync();
}

void BookSection::addButtonClicked() {
  m_bookAddDialog->createBook();
}

void BookSection::searchTextChanged() {
  QVariantMap filters = m_model->filters();
  QString text = ui->searchLineEdit->text();
  if (!text.isEmpty()) {
    filters["title"] = text;
  } else {
    filters.remove("title");
  }

  m_model->setFilters(filters);
}

void BookSection::setBooksCount(qint32 booksCount) {
  ui->numberOfBooksLabel->setText(QString::number(booksCount));
}

void BookSection::deleteButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->bookListView->selectionModel()->selectedIndexes();

  for (const QModelIndex &index : selectedIndexes) {
    quint32 bookId = m_model->data(index, BookRestModel::IdRole).toUInt();

    BookController controller;
    controller.deleteResource(bookId)
      .then(this, [this, index](auto) {
      if (index.isValid()) {
        m_model->removeRow(index.row());
      }
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

void BookSection::reloadPage() {
  m_loadPageTimer.start();
}

void BookSection::currentChanged(const QModelIndex &current,
                                 const QModelIndex & /*previous*/) {
  ui->showDetailsButton->setEnabled(current.isValid());

  quint32 bookId = current.data(BookRestModel::IdRole).toUInt();
  QString bookTitle = current.data(BookRestModel::TitleRole).toString();
  auto cover = current.data(BookRestModel::CoverRole).value<QPixmap>();

  ui->bookId->setText(QString::number(bookId));
  ui->bookTitle->setText(bookTitle);
  if (!cover.isNull()) {
    ui->bookCover->setPixmap(cover);
  } else {
    QUrl coverUrl = current.data(BookRestModel::CoverUrlRole).toUrl();
    WidgetUtils::asyncLoadImage(ui->bookCover, coverUrl);
  }
}

void BookSection::showDetailsButtonClicked() {
  bool converted;
  quint32 bookId = ui->bookId->text().toUInt(&converted);
  if (converted) {
    emit bookDetailsRequested(bookId);
  }
}

void BookSection::viewChangeButtonToggled(QAbstractButton *button,
                                          bool checked) {
  if (!checked) {
    return;
  }

  if (button == ui->gridViewButton) {
    m_model->shouldFetchImages(true);
    /* NOTE: Even if it's not visible it will call `fetchMore`
     * more times than necessary. This line `fixes` the problem 
     */
    ui->bookTableView->setMaximumSize(0, 0);
    ui->stackedWidget->setCurrentWidget(ui->gridViewPage);
  } else {
    m_model->shouldFetchImages(false);
    ui->bookTableView->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->stackedWidget->setCurrentWidget(ui->tableViewPage);
  }
}

void BookSection::bookRated(const QModelIndex &topLeft,
                            const QModelIndex & /*topRight*/,
                            const QList<int> &roles) {
  const QModelIndex &index = topLeft;

  if (roles.contains(BookRestModel::RatingRole)) {
    const BookCard &bookCard = m_model->get(index.row());
    BookRatingController::rateBook(bookCard.bookId(), bookCard.rating());
  }
}
