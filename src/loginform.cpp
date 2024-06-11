#include <QKeyEvent>
#include <QSettings>
#include <QValidator>

#include "common/error.h"
#include "loginform.h"
#include "ui_loginform.h"

#include "errormessagepopup.h"

#include "network/networkerror.h"
#include "resourcemanager.h"

LoginForm::LoginForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginForm),
      m_authentication(ResourceManager::networkManager()),
      m_usernameErrorPopup(new ErrorMessagePopup(this)),
      m_passwordErrorPopup(new ErrorMessagePopup(this)) {
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

  if (username.isEmpty() || password.isEmpty()) {
    if (username.isEmpty()) {
      m_usernameErrorPopup->showMessage(ui->username, "This field is required");
    }
    if (password.isEmpty()) {
      m_passwordErrorPopup->showMessage(ui->password, "This field is required");
    }
    return;
  }

  int port = portString.toInt();

  QUrl hostUrl;
  hostUrl.setHost(host);
  hostUrl.setPort(port);
  hostUrl.setScheme("http");

  ui->loginButton->setDisabled(true);
  m_authentication.login(hostUrl, username, password)
    .then([this, hostUrl](const QByteArray &token) {
    if (ui->rememberMeCheckBox->isChecked()) {
      QSettings settings;
      settings.setValue("credentials/token", token);
      settings.setValue("server/host", hostUrl.host());
      settings.setValue("server/port", hostUrl.port());
    }

    emit logged();
  }).onFailed(this, [this](const NetworkError &err) {
    ui->loginButton->setEnabled(true);
    handleError(this, err);
  });
}
