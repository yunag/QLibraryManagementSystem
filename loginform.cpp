#include <QMessageBox>
#include <QSqlError>

#include <QFile>
#include <QKeyEvent>
#include <QSettings>
#include <QValidator>

#include "./ui_loginform.h"
#include "librarydatabase.h"
#include "loginform.h"
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
  QString username = ui->userNameLineEdit->text();

  QString password = ui->passwordLineEdit->text();

  int port = portString.toInt();

  QSettings settings;
  qDebug() << settings.fileName();
  ensureSettings();

  settings.beginGroup("DatabaseCredentials");

  QString dbUserName = settings.value("username").toString();
  QString dbPassword = settings.value("password").toString();
  QString dbName = settings.value("dbname").toString();

  settings.endGroup();

  ui->loginButton->setEnabled(false);
  LibraryDatabase::open(dbName, dbUserName, dbPassword, host, port)
    .then([this]() { emit logged(); })
    .onFailed(this, [this](const QSqlError &e) {
      QMessageBox::warning(this, tr("Database connection failure"),
                           tr("Unable to connect to database: ") + e.text());

      ui->loginButton->setEnabled(true);
    });
}

void LoginForm::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Return && ui->loginButton->isEnabled()) {
    ui->loginButton->animateClick();
  }
}

void LoginForm::ensureSettings() {
  QSettings settings;

  if (!settings.contains("DatabaseCredentials")) {
    settings.beginGroup("DatabaseCredentials");

    settings.setValue("dbname", "qlibrarydb");
    settings.setValue("username", "root");
    settings.setValue("password", "");

    settings.endGroup();
  }
}
