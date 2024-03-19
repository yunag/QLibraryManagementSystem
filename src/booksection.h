#ifndef BOOKSECTION_H
#define BOOKSECTION_H

#include <QFuture>
#include <QWidget>

class QStandardItemModel;

class BookAddDialog;
class BookSectionDAO;
class SearchFilterDialog;

namespace Ui {
class BookSection;
}

class BookSection : public QWidget {
  Q_OBJECT

public:
  explicit BookSection(QWidget *parent = nullptr);
  ~BookSection();

  void loadBooks();

signals:
  void bookDetailsRequested(quint32 bookId);

protected:
  virtual void resizeEvent(QResizeEvent *event) override;

private:
  void loadPage(qint32 pageNumber);
  void reloadPage();

  void distributeGridSize();
  void updateLastSync();
  QFuture<void> updateNumberOfBooks();
  void hideItems(quint32 itemStart = 0);

  void setBooksCount(qint32 booksCount);

private slots:
  void synchronizeNowButtonClicked();
  void searchTextChanged(const QString &text);
  void addButtonClicked();
  void bookInsertedHandle();
  void saveChanges();
  void deleteButtonClicked();

private:
  Ui::BookSection *ui;

  BookAddDialog *m_bookAddDialog;
  SearchFilterDialog *m_searchFilterDialog;
  BookSectionDAO *m_dao;

  qint32 m_booksCount;
};

#endif  // BOOKSECTION_H
