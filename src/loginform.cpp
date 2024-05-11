#include <QKeyEvent>
#include <QValidator>

#include "common/error.h"
#include "loginform.h"
#include "ui_loginform.h"

#include "network/networkerror.h"

#include "libraryapplication.h"

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginForm), m_authentication(App->network()) {
  ui->setupUi(this);

  ui->port->setValidator(new QIntValidator(0, 65535, this));

  connect(ui->loginButton, &QPushButton::clicked, this,
          &LoginForm::loginButtonClicked);
}

LoginForm::~LoginForm() {
  delete ui;
}

static QString usePlaceholderIfEmpty(QLineEdit *lineEdit) {
  QString text = lineEdit->text();

  if (text.isEmpty()) {
    text = lineEdit->placeholderText();
    lineEdit->setText(text);
  }
  return text;
}

void LoginForm::loginButtonClicked() {
  QString portString = usePlaceholderIfEmpty(ui->port);
  QString host = usePlaceholderIfEmpty(ui->host);

  QString username = ui->username->text();
  QString password = ui->password->text();

  int port = portString.toInt();

  QUrl hostUrl;
  hostUrl.setHost(host);
  hostUrl.setPort(port);
  hostUrl.setScheme("http");

  m_authentication.login(hostUrl, username, password)
    .then([this]() { emit logged(); })
    .onFailed(this,
              [this](const NetworkError &err) { handleError(this, err); });
}
