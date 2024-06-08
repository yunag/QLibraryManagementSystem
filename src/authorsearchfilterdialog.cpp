#include "authorsearchfilterdialog.h"
#include "ui_authorsearchfilterdialog.h"

#include "model/authorrestmodel.h"

AuthorSearchFilterDialog::AuthorSearchFilterDialog(AuthorRestModel *model,
                                                   QWidget *parent)
    : QDialog(parent), ui(new Ui::AuthorSearchFilterDialog), m_model(model) {
  ui->setupUi(this);

  ui->sortByComboBox->addItem("id", "id");
  ui->sortByComboBox->addItem("first name", "firstname");
  ui->sortByComboBox->addItem("last name", "lastname");
}

AuthorSearchFilterDialog::~AuthorSearchFilterDialog() {
  delete ui;
}

void AuthorSearchFilterDialog::accept() {
  QString field = ui->sortByComboBox->currentData().toString();
  QString orderBy = field;

  if (ui->ascRadioButton->isChecked()) {
    orderBy += "-asc";
  }
  m_model->setOrderBy(orderBy);
  QDialog::accept();
}
