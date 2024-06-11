#ifndef LIBRARYMAINWINDOW_H
#define LIBRARYMAINWINDOW_H

#include <QMainWindow>

#include "schema/author.h"

class BookSection;
class AuthorSection;
class CategorySection;
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
  void requestBookDetails(quint32 id);
  void requestAuthorDetails(quint32 id);

signals:
  void closed();
  void authorsPickerFinished(const QList<Author> &author);

private slots:
  void logged();
  void currentWidgetChanged(int index);

protected:
  void closeEvent(QCloseEvent *event) override;
  void changeEvent(QEvent *event) override;

private:
  Ui::LibraryMainWindow *ui;

  bool m_initialized = false;

  BookDetailsDialog *m_bookDetails;
  AuthorDetailsDialog *m_authorDetails;

  LoginForm *m_loginForm;
  BookSection *m_bookSection;
  AuthorSection *m_authorSection;
  CategorySection *m_categorySection;
};

#endif  // LIBRARYMAINWINDOW_H
