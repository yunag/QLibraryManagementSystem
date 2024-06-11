#ifndef LIBRARYMAINWINDOW_H
#define LIBRARYMAINWINDOW_H

#include <QMainWindow>

#include "schema/author.h"

class BookSection;
class AuthorSection;
class LoginForm;
class BookDetailsDialog;
class AuthorDetailsDialog;

namespace Ui {
class LibraryMainWindow;
}

class LibraryMainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit LibraryMainWindow(QWidget *parent = nullptr);
  ~LibraryMainWindow() override;

  void showLoginForm();
  void requestAuthorsPicker();

signals:
  void closed();
  void authorsPickerFinished(const QList<Author> &author);

private slots:
  void logged();
  void currentWidgetChanged(int index);

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::LibraryMainWindow *ui;

  BookDetailsDialog *m_bookDetails;
  AuthorDetailsDialog *m_authorDetails;

  QWidget *m_centralWidget;

  LoginForm *m_loginForm;
  BookSection *m_bookSection;
  AuthorSection *m_authorSection;
};

#endif  // LIBRARYMAINWINDOW_H
