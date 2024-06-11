#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>

#include "network/userauthentication.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class LoginForm;
}

QT_END_NAMESPACE

class ErrorMessagePopup;

class LoginForm : public QWidget {
  Q_OBJECT

public:
  LoginForm(QWidget *parent = nullptr);
  ~LoginForm() override;

signals:
  void logged();

private slots:
  void loginButtonClicked();

private:
  Ui::LoginForm *ui;

  UserAuthentication m_authentication;
  ErrorMessagePopup *m_usernameErrorPopup;
  ErrorMessagePopup *m_passwordErrorPopup;
};
#endif  // LOGINFORM_H
