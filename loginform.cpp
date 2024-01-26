#include <QMainWindow>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>

#include <QKeyEvent>
#include <QValidator>
#include <qnamespace.h>

#include "./ui_loginform.h"
#include "loginform.h"
#include "qlibrarydatabase.h"
#include "widgetutils.h"

LoginForm::LoginForm(QWidget *parent) : QWidget(parent), ui(new Ui::LoginForm) {
  ui->setupUi(this);

  ui->portLineEdit->setValidator(new QIntValidator(0, 65535, this));

  connect(ui->loginButton, &QPushButton::clicked, this,
          &LoginForm::loginButtonClicked);
}

LoginForm::~LoginForm() {
  delete ui;
}

void LoginForm::loginButtonClicked() {
  QString portString = usePlaceholderIfEmpty(ui->portLineEdit);
  QString host = usePlaceholderIfEmpty(ui->hostLineEdit);
  QString username = ui->userNameLineEdit->text().isEmpty()
                       ? "root"
                       : ui->userNameLineEdit->text();

  QString password = ui->passwordLineEdit->text();

  int port = portString.toInt();

  ui->loginButton->setEnabled(false);
  LibraryDatabase::open("qlibrarydb", username, password, host, port)
    .then([this]() { emit logged(); })
    .onFailed(this, [this](const QSqlError &e) {
      QMessageBox::warning(this, tr("Database connection failure"),
                           tr("Unable to connect to database: ") + e.text());

      ui->loginButton->setEnabled(true);
    });
}

void LoginForm::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Return) {
    ui->loginButton->animateClick();
  }
}
