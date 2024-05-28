#include "searchfilterdialog.h"
#include "ui_searchfilterdialog.h"

#include "model/bookrestmodel.h"

SearchFilterDialog::SearchFilterDialog(BookRestModel *model, QWidget *parent)
    : QDialog(parent), ui(new Ui::SearchFilterDialog), m_model(model) {
  ui->setupUi(this);

  ui->sortByComboBox->addItem("id", "id");
  ui->sortByComboBox->addItem("title", "title");
  ui->sortByComboBox->addItem("rating", "rating");
}

SearchFilterDialog::~SearchFilterDialog() {
  delete ui;
}

void SearchFilterDialog::accept() {
  QString field = ui->sortByComboBox->currentData().toString();
  QString orderBy = field;

  if (ui->ascRadioButton->isChecked()) {
    orderBy += "-asc";
  }

  m_model->setOrderBy(orderBy);

  QDialog::accept();
}

void SearchFilterDialog::open() {
  QDialog::open();
}
