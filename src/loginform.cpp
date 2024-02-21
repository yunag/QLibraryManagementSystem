#include <QMessageBox>
#include <QSqlError>

#include <QFile>
#include <QKeyEvent>
#include <QSettings>
#include <QValidator>

#include "database/librarydatabase.h"
#include "forms/ui_loginform.h"
#include "loginform.h"
#include "user.h"
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

  QString username = ui->usernameLineEdit->text();
  QString password = ui->passwordLineEdit->text();

  int port = portString.toInt();

  QSettings settings;
  settings.beginGroup("DatabaseCredentials");

  QString dbUserName = settings.value("username").toString();
  QString dbPassword = settings.value("password").toString();
  QString dbName = settings.value("dbname").toString();

  settings.endGroup();

  /* TODO: Remove it later */
  username = username.isEmpty() ? "admin" : username;
  password = password.isEmpty() ? "admin" : password;

  ui->loginButton->setEnabled(false);
  LibraryDatabase::open(dbName, dbUserName, dbPassword, host, port)
    .then(QtFuture::Launch::Async,
          [this, username, password]() {
            if (User::validate(username, password).result()) {
              emit logged();
            }
          })
    .onFailed(this,
              [this](const QSqlError &e) {
                QMessageBox::warning(this, tr("Database connection failure"),
                                     tr("Unable to connect to database: ") +
                                       e.text());
              })
    .then(this, [this]() { ui->loginButton->setEnabled(true); });
}

void LoginForm::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Return && ui->loginButton->isEnabled()) {
    ui->loginButton->animateClick();
  }
}
