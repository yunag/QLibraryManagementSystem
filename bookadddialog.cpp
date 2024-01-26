#include <QAbstractProxyModel>
#include <QCompleter>
#include <QStandardItemModel>

#include "bookadddialog.h"
#include "ui_bookadddialog.h"

static void setupCompleter(QCompleter *completer) {
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setFilterMode(Qt::MatchContains);
  completer->setCompletionMode(QCompleter::PopupCompletion);
}

static void applyCompletion(QComboBox *combobox, QListWidget *listWidget) {
  QCompleter *completer = combobox->completer();
  QStandardItemModel *model =
    qobject_cast<QStandardItemModel *>(completer->model());

  if (!completer->setCurrentRow(0)) {
    /* No match */
    return;
  }

  QAbstractProxyModel *proxyModel =
    completer->findChild<QAbstractProxyModel *>();
  Q_ASSERT(proxyModel != nullptr);

  QModelIndex index = proxyModel->mapToSource(completer->currentIndex());

  listWidget->addItem(completer->currentCompletion());
  combobox->removeItem(index.row());
}

static void removeItem(const QListWidgetItem *item, const QListWidget *list,
                       QComboBox *combobox) {
  combobox->addItem(item->text());

  QModelIndex index = list->indexFromItem(item);
  list->model()->removeRow(index.row());
}

BookAddDialog::BookAddDialog(QWidget *parent)
    : QWidget(parent), ui(new Ui::BookAddDialog) {
  ui->setupUi(this);

  setupCompleter(ui->categoriesComboBox->completer());
  setupCompleter(ui->authorsComboBox->completer());

  QLineEdit *categoriesLineEdit = ui->categoriesComboBox->lineEdit();
  QLineEdit *authorsLineEdit = ui->authorsComboBox->lineEdit();

  connect(categoriesLineEdit, &QLineEdit::returnPressed, this,
          &BookAddDialog::categoriesReturnPressed);
  connect(authorsLineEdit, &QLineEdit::returnPressed, this,
          &BookAddDialog::authorsReturnPressed);

  connect(ui->categoriesListWidget, &QListWidget::itemDoubleClicked, this,
          &BookAddDialog::categoriesItemDoubleClicked);
  connect(ui->authorsListWidget, &QListWidget::itemDoubleClicked, this,
          &BookAddDialog::authorsItemDoubleClicked);
}

BookAddDialog::~BookAddDialog() {
  delete ui;
}

void BookAddDialog::categoriesReturnPressed() {
  applyCompletion(ui->categoriesComboBox, ui->categoriesListWidget);
}

void BookAddDialog::authorsReturnPressed() {
  applyCompletion(ui->authorsComboBox, ui->authorsListWidget);
}

void BookAddDialog::authorsItemDoubleClicked(QListWidgetItem *item) {
  removeItem(item, ui->authorsListWidget, ui->authorsComboBox);
}

void BookAddDialog::categoriesItemDoubleClicked(QListWidgetItem *item) {
  removeItem(item, ui->categoriesListWidget, ui->categoriesComboBox);
}
