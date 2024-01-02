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

LoginForm::LoginForm(QLibraryDatabase &database, QWidget *parent)
    : QWidget(parent), m_database(database), ui(new Ui::LoginForm) {
  ui->setupUi(this);

  ui->portLineEdit->setValidator(new QIntValidator(0, 65535, this));

  connect(ui->loginButton, &QPushButton::clicked, this,
          &LoginForm::loginButtonClicked, Qt::QueuedConnection);
  connect(this, &LoginForm::errorOccured, this, &LoginForm::errorHandler,
          Qt::QueuedConnection);
}

LoginForm::~LoginForm() { delete ui; }

void LoginForm::errorHandler(QString error) {
  qDebug() << "ErrorHandler:" << QThread::currentThread();

  QMessageBox::warning(this, tr("Database connection failure"),
                       tr("Unable to connect to database: ") + error);
}

void LoginForm::loginButtonClicked() {
  QString portString = usePlaceholderIfEmpty(ui->portLineEdit);
  QString host = usePlaceholderIfEmpty(ui->hostLineEdit);
  QString username = ui->userNameLineEdit->text().isEmpty()
                         ? "root"
                         : ui->userNameLineEdit->text();

  QString password = ui->passwordLineEdit->text();

  int port = portString.toInt();

  m_database.open("qlibrarydb", username, password, host, port)
      .then([this](bool ok) {
        if (ok) {
          emit logged();
        } else {
          QString error = m_database.lastError().result().text();

          emit errorOccured(error);
        }
      });
}
void LoginForm::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Return) {
    ui->loginButton->animateClick();
  }
}
