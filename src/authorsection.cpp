#include <QButtonGroup>
#include <QScrollBar>

#include "authorsection.h"
#include "model/authorrestmodel.h"
#include "ui_authorsection.h"

AuthorSection::AuthorSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::AuthorSection) {
  ui->setupUi(this);

  m_loadPageTimer.setInterval(0);
  m_loadPageTimer.setSingleShot(true);

  connect(&m_loadPageTimer, &QTimer::timeout, this, [this]() {
    m_model->reset();
    m_model->reload();
  });

  auto *pagination = new Pagination(this);
  pagination->setPerPage(60);

  ui->pagination->setPagination(pagination);

  m_model->setOrderBy("id");
  m_model->shouldFetchImages(true);
  m_model->setPagination(pagination);

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
          [this](QAbstractButton *button, bool checked) {
    if (!checked) {
      return;
    }

    if (button == ui->gridViewButton) {
      m_model->shouldFetchImages(true);
      ui->stackedWidget->setCurrentWidget(ui->gridViewPage);
    } else {
      m_model->shouldFetchImages(false);
      ui->stackedWidget->setCurrentWidget(ui->tableViewPage);
    }
  });

  ui->authorListView->verticalScrollBar()->setSingleStep(8);
  ui->authorListView->horizontalScrollBar()->setDisabled(true);

  ui->authorListView->setAcceptDrops(false);
  ui->authorListView->setDragDropMode(QAbstractItemView::NoDragDrop);

  QAction *action = ui->searchLineEdit->addAction(QIcon(":/icons/searchIcon"),
                                                  QLineEdit::LeadingPosition);

  // connect(action, &QAction::triggered, m_searchFilterDialog,
  //         &AuthorSectionSearchFilterDialog::open);

  auto handleSortFilterChange = [this]() {
    m_model->pagination()->setCurrentPage(0);
    reloadPage();
  };
  connect(m_model, &AuthorRestModel::orderByChanged, this,
          handleSortFilterChange);
  connect(m_model, &AuthorRestModel::filtersChanged, this,
          handleSortFilterChange);

  connect(pagination, &Pagination::currentPageChanged, m_model,
          [this](int page) { reloadPage(); });
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

  connect(ui->deleteButton, &QPushButton::clicked, this,
          &AuthorSection::deleteButtonClicked);
  //  connect(m_bookAddDialog, &AuthorAddDialog::edited, this,
  //          &AuthorSection::reloadPage);
  connect(ui->addButton, &QPushButton::clicked, this,
          &AuthorSection::addButtonClicked);
  connect(ui->updateButton, &QPushButton::clicked, this,
          &AuthorSection::updateButtonClicked);
  connect(ui->synchronizeNowButton, &QPushButton::clicked, this,
          &AuthorSection::synchronizeNowButtonClicked);
  connect(ui->searchLineEdit, &QLineEdit::textChanged, this,
          &AuthorSection::searchTextChanged);
}

AuthorSection::~AuthorSection() {
  delete ui;
}
