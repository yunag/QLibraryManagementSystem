#ifndef LIBRARYMAINWINDOW_H
#define LIBRARYMAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>

#include "qlibrarydatabase.h"

class BookSection;
class LoginForm;

namespace Ui {
class LibraryMainWindow;
}

class LibraryMainWindow : public QMainWindow, public QSqlDatabase {
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
  QLibraryDatabase m_database;

  QWidget *m_currentSection;
  LoginForm *m_loginForm;
  BookSection *m_booksSection;
};

#endif // LIBRARYMAINWINDOW_H
