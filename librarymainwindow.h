#ifndef LIBRARYMAINWINDOW_H
#define LIBRARYMAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

class BookSection;
class LoginForm;
class LibrarySideMenu;

namespace Ui {
class LibraryMainWindow;
}

class LibraryMainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit LibraryMainWindow(QWidget *parent = nullptr);
  ~LibraryMainWindow();

  void showLoginForm();

public slots:
  void onLogged();
  void booksButtonClicked();

private:
  Ui::LibraryMainWindow *ui;

  QWidget *m_currentSection;

  LoginForm *m_loginForm;
  BookSection *m_booksSection;
};

#endif  // LIBRARYMAINWINDOW_H
