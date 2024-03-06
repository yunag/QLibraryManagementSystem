#ifndef LIBRARYMAINWINDOW_H
#define LIBRARYMAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

class BookSection;
class LoginForm;
class LibrarySideMenu;
class BookDetailsDialog;
class AuthorDetailsDialog;

namespace Ui {
class LibraryMainWindow;
}

class LibraryMainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit LibraryMainWindow(QWidget *parent = nullptr);
  ~LibraryMainWindow();

  void showLoginForm();

signals:
  void closed();

public slots:
  void onLogged();
  void booksButtonClicked();

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::LibraryMainWindow *ui;

  BookDetailsDialog *m_bookDetails;
  AuthorDetailsDialog *m_authorDetails;

  LoginForm *m_loginForm;
  BookSection *m_booksSection;
};

#endif  // LIBRARYMAINWINDOW_H
