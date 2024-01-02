#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QPointer>
#include <QWidget>

class QLibraryDatabase;

QT_BEGIN_NAMESPACE
namespace Ui {
class LoginForm;
}
QT_END_NAMESPACE

class LoginForm : public QWidget {
  Q_OBJECT

public:
  LoginForm(QLibraryDatabase &db, QWidget *parent = nullptr);
  ~LoginForm();

protected:
  void keyPressEvent(QKeyEvent *event) override;

signals:
  void logged();
  void errorOccured(QString error);

private slots:
  void loginButtonClicked();
  void errorHandler(QString error);

private:
  QLibraryDatabase &m_database;
  Ui::LoginForm *ui;
};
#endif // LOGINFORM_H
