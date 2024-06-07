#include "authoradddialog.h"
#include "ui_authoradddialog.h"

AuthorAddDialog::AuthorAddDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AuthorAddDialog) {
  ui->setupUi(this);
}

void AuthorAddDialog::createAuthor() {
  init();

  open();
}

AuthorAddDialog::~AuthorAddDialog() {
  delete ui;
}

void AuthorAddDialog::init() {
  ui->firstName->clear();
  ui->lastName->clear();
  ui->bio->clear();
  ui->authorImage->setPixmap(QPixmap(":/images/DefaultBookCover"));
}
