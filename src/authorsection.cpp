#include <QButtonGroup>
#include <QScrollBar>
#include <QSettings>

#include "authorsection.h"
#include "ui_authorsection.h"

#include "common/widgetutils.h"
#include "controllers/authorcontroller.h"

#include "authoradddialog.h"
#include "authorsearchfilterdialog.h"

#include "delegate/authoricondelegate.h"
#include "model/authorrestmodel.h"

#include <chrono>
using namespace std::chrono_literals;

AuthorSection::AuthorSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::AuthorSection),
      m_model(new AuthorRestModel(this)),
      m_authorAddDialog(new AuthorAddDialog(this)),
      m_searchFilterDialog(new AuthorSearchFilterDialog(m_model, this)) {

  ui->setupUi(this);

  m_loadPageTimer.setInterval(0);
  m_loadPageTimer.setSingleShot(true);

  m_searchTimer.setInterval(300ms);
  m_searchTimer.setSingleShot(true);

  connect(&m_searchTimer, &QTimer::timeout, this,
          &AuthorSection::searchTextChanged);

  connect(&m_loadPageTimer, &QTimer::timeout, this, [this]() {
    m_model->reset();
    m_model->reload();
  });

  auto *pagination = new Pagination(this);
  pagination->setPerPage(25);

  ui->pagination->setPagination(pagination);

  m_model->setOrderBy("firstname");
  m_model->shouldFetchImages(true);
  m_model->setPagination(pagination);

  ui->authorListView->setItemDelegate(new AuthorIconDelegate);
  ui->authorListView->setItemSize(AuthorIconDelegate::sizeHint());
  ui->authorListView->setModel(m_model);

  ui->authorImage->setAspectRatio(Qt::KeepAspectRatio);

  ui->authorTableView->setModel(m_model);
  ui->authorTableView->setSelectionModel(ui->authorListView->selectionModel());

  connect(ui->authorTableView->selectionModel(),
          &QItemSelectionModel::currentChanged, this,
          &AuthorSection::currentChanged);

  QHeaderView *horizontalHeader = ui->authorTableView->horizontalHeader();
  horizontalHeader->setSectionResizeMode(AuthorRestModel::FirstNameHeader,
                                         QHeaderView::Stretch);
  horizontalHeader->setSectionResizeMode(AuthorRestModel::LastNameHeader,
                                         QHeaderView::Stretch);

  /* Autoexclusive buttons must be in the same QButtonGroup */
  auto *buttonGroup = new QButtonGroup(this);
  buttonGroup->addButton(ui->gridViewButton);
  buttonGroup->addButton(ui->tableViewButton);

  connect(buttonGroup, &QButtonGroup::buttonToggled, this,
          &AuthorSection::viewChangeButtonToggled);

  ui->authorListView->verticalScrollBar()->setSingleStep(8);
  ui->authorListView->horizontalScrollBar()->setDisabled(true);

  ui->authorListView->setAcceptDrops(false);
  ui->authorListView->setDragDropMode(QAbstractItemView::NoDragDrop);

  QSettings settings;
  ui->splitter->restoreState(
    settings.value("authorsection/splitter").toByteArray());

  QAction *action = ui->searchLineEdit->addAction(QIcon(":/icons/searchIcon"),
                                                  QLineEdit::LeadingPosition);

  connect(action, &QAction::triggered, m_searchFilterDialog,
          &AuthorSearchFilterDialog::open);

  auto handleSortFilterChange = [this]() {
    m_model->pagination()->setCurrentPage(0);
    reloadPage();
  };
  connect(m_model, &AuthorRestModel::orderByChanged, this,
          handleSortFilterChange);
  connect(m_model, &AuthorRestModel::filtersChanged, this,
          handleSortFilterChange);

  connect(pagination, &Pagination::currentPageChanged, m_model,
          [this](auto) { reloadPage(); });
  connect(pagination, &Pagination::totalCountChanged, this,
          &AuthorSection::setAuthorsCount);

  ui->showDetailsButton->setEnabled(false);
  connect(ui->showDetailsButton, &QPushButton::clicked, this,
          &AuthorSection::showDetailsButtonClicked);
  connect(ui->authorListView, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
    quint32 authorId = m_model->data(index, AuthorRestModel::IdRole).toUInt();
    emit authorDetailsRequested(authorId);
  });

  connect(m_authorAddDialog, &AuthorAddDialog::edited, this,
          &AuthorSection::reloadPage);
  connect(ui->deleteButton, &QPushButton::clicked, this,
          &AuthorSection::deleteButtonClicked);
  connect(ui->addButton, &QPushButton::clicked, this,
          &AuthorSection::addButtonClicked);
  connect(ui->updateButton, &QPushButton::clicked, this,
          &AuthorSection::updateButtonClicked);
  connect(ui->synchronizeNowButton, &QPushButton::clicked, this,
          &AuthorSection::synchronizeNowButtonClicked);
  connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
          [this](auto) { m_searchTimer.start(); });
}

AuthorSection::~AuthorSection() {
  QSettings settings;
  settings.setValue("authorsection/splitter", ui->splitter->saveState());
  delete ui;
}

void AuthorSection::updateLastSync() {
  QDateTime dateTime = QDateTime::currentDateTime();
  QString formattedDate = QLocale().toString(dateTime);

  ui->lastSyncLabel->setText(formattedDate);
}

void AuthorSection::loadAuthors() {
  updateLastSync();

  reloadPage();
}

void AuthorSection::synchronizeNowButtonClicked() {
  reloadPage();
  updateLastSync();
}

void AuthorSection::addButtonClicked() {
  m_authorAddDialog->createAuthor();
}

void AuthorSection::searchTextChanged() {
  QVariantMap filters = m_model->filters();
  QString text = ui->searchLineEdit->text();
  if (!text.isEmpty()) {
    filters["name"] = text;
  } else {
    filters.remove("title");
  }

  m_model->setFilters(filters);
}

void AuthorSection::setAuthorsCount(qint32 authorsCount) {
  ui->numberOfAuthorsLabel->setText(QString::number(authorsCount));
}

void AuthorSection::deleteButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->authorListView->selectionModel()->selectedIndexes();

  for (const QModelIndex &index : selectedIndexes) {
    quint32 bookId = m_model->data(index, AuthorRestModel::IdRole).toUInt();

    //AuthorController::deleteById(bookId)
    //  .then(this, [this, index](auto) {
    //  if (index.isValid()) {
    //    m_model->removeRow(index.row());
    //  }
    //}).onFailed(this, [this](const NetworkError &err) {
    //  handleError(this, err);
    //});
  }
}

void AuthorSection::updateButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->authorListView->selectionModel()->selectedIndexes();
  if (selectedIndexes.isEmpty()) {
    return;
  }

  QModelIndex index = selectedIndexes.first();
  quint32 id = index.data(AuthorRestModel::IdRole).toUInt();

  m_authorAddDialog->editAuthor(id);
}

void AuthorSection::reloadPage() {
  m_loadPageTimer.start();
}

void AuthorSection::currentChanged(const QModelIndex &current,
                                   const QModelIndex & /*previous*/) {
  ui->showDetailsButton->setEnabled(current.isValid());

  quint32 authorId = current.data(AuthorRestModel::IdRole).toUInt();
  QString firstName = current.data(AuthorRestModel::FirstNameRole).toString();
  QString lastName = current.data(AuthorRestModel::LastNameRole).toString();
  auto image = current.data(AuthorRestModel::ImageRole).value<QPixmap>();

  ui->authorId->setText(QString::number(authorId));
  ui->authorFullname->setText(firstName + " " + lastName);
  if (!image.isNull()) {
    ui->authorImage->setPixmap(image);
  } else {
    QUrl imageUrl = current.data(AuthorRestModel::ImageUrlRole).toUrl();
    WidgetUtils::asyncLoadImage(ui->authorImage, imageUrl);
  }
}

void AuthorSection::showDetailsButtonClicked() {
  bool converted;
  quint32 bookId = ui->authorId->text().toUInt(&converted);
  if (converted) {
    emit authorDetailsRequested(bookId);
  }
}

void AuthorSection::viewChangeButtonToggled(QAbstractButton *button,
                                            bool checked) {
  if (!checked) {
    return;
  }

  if (button == ui->gridViewButton) {
    m_model->shouldFetchImages(true);
    /* NOTE: Even if it's not visible it will call `fetchMore`
     * more times than necessary. This line `fixes` the problem 
     */
    ui->authorTableView->setMaximumSize(0, 0);
    ui->stackedWidget->setCurrentWidget(ui->gridViewPage);
  } else {
    m_model->shouldFetchImages(false);
    ui->authorTableView->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->stackedWidget->setCurrentWidget(ui->tableViewPage);
  }
}
