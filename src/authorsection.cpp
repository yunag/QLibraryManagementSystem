#include <QButtonGroup>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QSettings>

#include <QFinalState>
#include <QState>
#include <QStateMachine>

#include "authorsection.h"
#include "common/error.h"
#include "ui_authorsection.h"

#include "libraryapplication.h"
#include "librarymainwindow.h"

#include "common/widgetutils.h"
#include "controllers/authorcontroller.h"

#include "authoradddialog.h"
#include "authorsearchfilterdialog.h"

#include "delegate/authoricondelegate.h"
#include "delegate/leftaligndelegate.h"

#include "model/authorlistmodel.h"
#include "model/authorrestmodel.h"

AuthorSection::AuthorSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::AuthorSection),
      m_model(new AuthorRestModel(this)),
      m_authorAddDialog(new AuthorAddDialog(this)),
      m_searchFilterDialog(new AuthorSearchFilterDialog(m_model, this)) {
  ui->setupUi(this);

  ui->authorPicker->setMaximumSize(0, 0);
  ui->authorPicker->setVisible(false);
  ui->authorPickerList->setHeaderText("Authors");

  m_loadPageTimer.setInterval(0);
  m_loadPageTimer.setSingleShot(true);

  QSettings settings;
  m_searchTimer.setInterval(
    settings.value("general/msSearchTimerUpdateSpeed").toInt());
  m_searchTimer.setSingleShot(true);

  connect(&m_searchTimer, &QTimer::timeout, this,
          &AuthorSection::searchTextChanged);

  connect(&m_loadPageTimer, &QTimer::timeout, this, [this]() {
    m_model->reset();
    m_model->reload();
  });

  connect(ui->authorPickerList, &HeaderListView::doubleClicked, this,
          [this](const QModelIndex &index) {
    ui->authorPickerList->model()->removeRow(index.row());
  });

  auto *pagination = new Pagination(this);
  pagination->setPerPage(25);

  ui->pagination->setPagination(pagination);

  m_model->setOrderBy("firstname");
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

  ui->authorListView->setDragDropMode(QAbstractItemView::NoDragDrop);
  ui->authorTableView->setDragDropMode(QAbstractItemView::NoDragDrop);

  ui->splitter->restoreState(
    settings.value("authorsection/splitter").toByteArray());

  QAction *action = ui->searchLineEdit->addAction(QIcon::fromTheme("searching"),
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
    quint32 id = m_model->data(index, AuthorRestModel::IdRole).toUInt();
    App->mainWindow()->requestAuthorDetails(id);
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

  connect(m_model, &AuthorRestModel::rowsInserted, this,
          &AuthorSection::authorListRowsInserted);
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

  if (!m_model->rowCount()) {
    reloadPage();
  }
}

void AuthorSection::synchronizeNowButtonClicked() {
  reloadPage();
  updateLastSync();
}

void AuthorSection::addButtonClicked() {
  m_authorAddDialog->createAuthor();
}

void AuthorSection::searchTextChanged() {
  m_searchFilterDialog->setSearchText(ui->searchLineEdit->text());
}

void AuthorSection::setAuthorsCount(qint32 authorsCount) {
  ui->numberOfAuthorsLabel->setText(QString::number(authorsCount));
}

void AuthorSection::deleteButtonClicked() {
  QModelIndexList selectedIndexes =
    ui->authorListView->selectionModel()->selectedIndexes();
  AuthorController controller;

  for (const QModelIndex &index : selectedIndexes) {
    quint32 id = m_model->data(index, AuthorRestModel::IdRole).toUInt();

    controller.deleteResource(id)
      .then(this, [this, index](auto) {
      if (index.isValid()) {
        m_model->removeRow(index.row());
      }

      Pagination *pagination = m_model->pagination();
      pagination->setTotalCount(pagination->totalCount() - 1);
    }).onFailed(this, [this](const NetworkError &err) {
      handleError(this, err);
    });
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
  quint32 id = ui->authorId->text().toUInt(&converted);
  if (converted) {
    App->mainWindow()->requestAuthorDetails(id);
  }
}

void AuthorSection::viewChangeButtonToggled(QAbstractButton *button,
                                            bool checked) {
  if (!checked) {
    return;
  }

  if (button == ui->gridViewButton) {
    /* NOTE: Even if it's not visible it will call `fetchMore`
     * more times than necessary. This line `fixes` the problem 
     */
    ui->authorTableView->setMaximumSize(0, 0);
    ui->stackedWidget->setCurrentWidget(ui->gridViewPage);
  } else {
    ui->authorTableView->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->stackedWidget->setCurrentWidget(ui->tableViewPage);
  }
}

void AuthorSection::toggleAuthorPickerMode() {
  auto *machine = new QStateMachine(this);
  machine->setGlobalRestorePolicy(QState::RestoreProperties);

  auto *animation =
    new QPropertyAnimation(ui->authorPicker, "maximumSize", machine);
  animation->setDuration(200);
  animation->setEasingCurve(QEasingCurve::InOutQuad);

  machine->addDefaultAnimation(animation);

  auto *model = new AuthorListModel(machine);
  connect(model, &AuthorListModel::rowsInserted, this,
          &AuthorSection::authorPickerListModifed);
  connect(model, &AuthorListModel::rowsRemoved, this,
          &AuthorSection::authorPickerListModifed);

  ui->authorPickerList->setModel(model);
  ui->authorPickerList->setItemDelegate(new LeftAlignDelegate);

  auto *s1 = new QState();
  auto *s2 = new QState();
  auto *s3 = new QFinalState();

  s1->assignProperty(ui->updateButton, "visible", false);
  s1->assignProperty(ui->deleteButton, "visible", false);
  s1->assignProperty(ui->addButton, "visible", false);
  s1->assignProperty(ui->authorPicker, "visible", true);

  s1->assignProperty(ui->authorListView, "dragDropMode",
                     QAbstractItemView::DragOnly);
  s1->assignProperty(ui->authorTableView, "dragDropMode",
                     QAbstractItemView::DragOnly);
  s1->assignProperty(ui->authorPickerList, "dragDropMode",
                     QAbstractItemView::DropOnly);

  s1->assignProperty(ui->authorPickerList, "defaultDropAction", Qt::CopyAction);
  s1->assignProperty(ui->authorPickerList, "showDropIndicator", true);
  s1->assignProperty(ui->authorPicker, "maximumSize",
                     QSize(200, QWIDGETSIZE_MAX));

  s1->addTransition(ui->confirmButton, &QPushButton::clicked, s2);
  s2->assignProperty(ui->authorPicker, "visible", true);
  s2->addTransition(s2, &QState::propertiesAssigned, s3);

  connect(machine, &QStateMachine::finished, this, [this, machine]() {
    auto *model =
      qobject_cast<AuthorListModel *>(ui->authorPickerList->model());
    QList<Author> authors;

    for (int row = 0; row < model->rowCount(); ++row) {
      QModelIndex index = model->index(row, 0);
      auto author =
        model->data(index, AuthorRestModel::ObjectRole).value<AuthorItem>();
      authors.push_back(std::move(author));
    }

    model->removeRows(0, model->rowCount());
    machine->deleteLater();

    emit authorsPickingFinished(authors);
  });

  machine->addState(s1);
  machine->addState(s2);
  machine->addState(s3);

  machine->setInitialState(s1);
  machine->start();
}

void AuthorSection::authorPickerListModifed(const QModelIndex & /*parent*/,
                                            int /*first*/, int /*last*/) {
  auto *model = qobject_cast<AuthorListModel *>(ui->authorPickerList->model());

  for (int i = 0; i < m_model->rowCount(); ++i) {
    QModelIndex index = m_model->index(i);
    quint32 authorId = m_model->data(index, AuthorRestModel::IdRole).toUInt();

    auto flags =
      m_model->data(index, AuthorRestModel::FlagsRole).value<Qt::ItemFlags>();

    flags.setFlag(Qt::ItemIsEnabled, !model->hasAuthorId(authorId));

    m_model->setData(index, QVariant::fromValue(flags),
                     AuthorRestModel::FlagsRole);
  }
}

void AuthorSection::authorListRowsInserted(const QModelIndex & /*parent*/,
                                           int first, int last) {
  auto *pickerListModel =
    qobject_cast<AuthorListModel *>(ui->authorPickerList->model());
  if (!pickerListModel) {
    return;
  }

  for (int i = first; i <= last; ++i) {
    QModelIndex index = m_model->index(i);
    quint32 authorId = m_model->data(index, AuthorRestModel::IdRole).toUInt();

    if (pickerListModel->hasAuthorId(authorId)) {
      auto flags =
        m_model->data(index, AuthorRestModel::FlagsRole).value<Qt::ItemFlags>();
      flags &= ~Qt::ItemIsEnabled;

      m_model->setData(index, QVariant::fromValue(flags),
                       AuthorRestModel::FlagsRole);
    }
  }
}
