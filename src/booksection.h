#ifndef BOOKSECTION_H
#define BOOKSECTION_H

#include <QFuture>
#include <QWidget>

#include <QTimer>

class QAbstractButton;

class BookCard;
class BookCardData;
class BookAddDialog;
class BookSearchFilterDialog;
class BookRestModel;

namespace Ui {
class BookSection;
}

class BookSection : public QWidget {
  Q_OBJECT

public:
  explicit BookSection(QWidget *parent = nullptr);
  ~BookSection() override;

  void loadBooks();

signals:
  void bookDetailsRequested(quint32 bookId);
  void authorsPickerRequested();

private slots:
  void reloadPage();
  void currentChanged(const QModelIndex &current, const QModelIndex &previous);

  void synchronizeNowButtonClicked();
  void searchTextChanged();
  void addButtonClicked();
  void showDetailsButtonClicked();
  void updateButtonClicked();
  void deleteButtonClicked();
  void viewChangeButtonToggled(QAbstractButton *button, bool checked);
  void bookRated(const QModelIndex &topLeft, const QModelIndex &topRight,
                 const QList<int> &roles);

private:
  void updateLastSync();
  void setBooksCount(int booksCount);

private:
  Ui::BookSection *ui;

  QTimer m_loadPageTimer;
  QTimer m_searchTimer;

  BookRestModel *m_model;
  BookAddDialog *m_bookAddDialog;
  BookSearchFilterDialog *m_searchFilterDialog;
};

#endif  // BOOKSECTION_H
