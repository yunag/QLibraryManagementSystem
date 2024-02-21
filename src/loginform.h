#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QSettings>
#include <QWidget>

class LibraryDatabase;

QT_BEGIN_NAMESPACE

namespace Ui {
class LoginForm;
}

QT_END_NAMESPACE

class LoginForm : public QWidget {
  Q_OBJECT

public:
  LoginForm(QWidget *parent = nullptr);
  ~LoginForm();

protected:
  void keyPressEvent(QKeyEvent *event) override;

signals:
  void logged();

private slots:
  void loginButtonClicked();

private:
  Ui::LoginForm *ui;
};
#endif  // LOGINFORM_H
