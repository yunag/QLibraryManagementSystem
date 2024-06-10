#ifndef LIBRARYMAINWINDOW_H
#define LIBRARYMAINWINDOW_H

#include <QMainWindow>

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

signals:
  void closed();

private slots:
  void authorsPickerRequested();
  void logged();
  void booksButtonClicked();
  void authorsButtonClicked();

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::LibraryMainWindow *ui;

  BookDetailsDialog *m_bookDetails;
  AuthorDetailsDialog *m_authorDetails;

  LoginForm *m_loginForm;
  BookSection *m_bookSection;
  AuthorSection *m_authorSection;
};

#endif  // LIBRARYMAINWINDOW_H
