#include "authorsection.h"
#include "forms/ui_authorsection.h"

AuthorSection::AuthorSection(QWidget *parent)
    : QWidget(parent), ui(new Ui::AuthorSection) {
  ui->setupUi(this);
}

AuthorSection::~AuthorSection() {
  delete ui;
}