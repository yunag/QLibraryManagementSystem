#include <QDateTime>

#include "controllers/categorycontroller.h"

#include "errormessagepopup.h"

#include "common/error.h"

#include "categorysection.h"
#include "network/networkerror.h"
#include "ui_categorysection.h"

CategorySection::CategorySection(QWidget *parent)
    : QWidget(parent), ui(new Ui::CategorySection),
      m_nameErrorMessage(new ErrorMessagePopup(this)) {
  ui->setupUi(this);

  auto rowsModifiedHandle = [this](const QModelIndex & /*parent*/,
                                   int /*first*/, int /*last*/) {
    ui->numberOfCategories->setText(QString::number(ui->categoryList->count()));
  };

  connect(ui->categoryList->model(), &QAbstractItemModel::rowsInserted, this,
          rowsModifiedHandle);
  connect(ui->categoryList->model(), &QAbstractItemModel::rowsRemoved, this,
          rowsModifiedHandle);

  connect(ui->addButton, &QPushButton::clicked, this,
          &CategorySection::addButtonClicked);
  connect(ui->deleteButton, &QPushButton::clicked, this,
          &CategorySection::deleteButtonClicked);
  connect(ui->synchronizeNowButton, &QPushButton::clicked, this,
          &CategorySection::syncronizeNowButtonClicked);
}

CategorySection::~CategorySection() {
  delete ui;
}

void CategorySection::loadCategories() {
  updateLastSync();

  if (ui->categoryList->count()) {
    return;
  }

  reload();
}

void CategorySection::updateLastSync() {
  QDateTime dateTime = QDateTime::currentDateTime();
  QString formattedDate = QLocale().toString(dateTime);

  ui->lastSyncLabel->setText(formattedDate);
}

void CategorySection::reload() {
  CategoryController controller;

  ui->categoryList->clear();

  controller.getCategories()
    .then(this, [this](const QList<Category> &categories) {
    for (const auto &category : categories) {
      auto *item = new QListWidgetItem(category.name);
      item->setData(IdRole, category.id);
      ui->categoryList->addItem(item);
    }
  }).onFailed([this](const NetworkError &err) { handleError(this, err); });
}

void CategorySection::deleteButtonClicked() {
  for (auto *item : ui->categoryList->selectedItems()) {
    quint32 id = item->data(IdRole).toUInt();

    CategoryController controller;
    controller.deleteResource(id).onFailed(
      [this](const NetworkError &err) { handleError(this, err); });

    delete ui->categoryList->takeItem(ui->categoryList->row(item));
  }
}

void CategorySection::addButtonClicked() {
  CategoryController controller;

  if (ui->categoryNameAdd->text().isEmpty()) {
    m_nameErrorMessage->showMessage(ui->categoryNameAdd,
                                    "Name must be non-empty string", 3000,
                                    ErrorMessagePopup::TopSide);
    return;
  }

  Category category;
  category.name = ui->categoryNameAdd->text();

  controller.create(category).then(this, [this]() {
    reload();
  }).onFailed([this](const NetworkError &err) { handleError(this, err); });
}

void CategorySection::syncronizeNowButtonClicked() {
  updateLastSync();
  reload();
}
