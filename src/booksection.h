#ifndef BOOKSECTION_H
#define BOOKSECTION_H

#include <QFuture>
#include <QWidget>

class QStandardItemModel;

class BookAddDialog;
class BookSectionDAO;
class BookDetailsDialog;
class SearchFilterDialog;

namespace Ui {
class BookSection;
}

class BookSection : public QWidget {
  Q_OBJECT

public:
  static constexpr int kItemsPerPage = 20;

public:
  explicit BookSection(QWidget *parent = nullptr);
  ~BookSection();

  void loadBooks();

protected:
  virtual void resizeEvent(QResizeEvent *event) override;

private:
  bool loadPage(qint32 pageNumber);
  void distributeGridSize();
  void updateLastSync();
  QFuture<void> updateNumberOfBooks();
  void hideItems(quint32 itemStart = 0);

  void setBooksCount(qint32 booksCount);

  void updatePageButtons(qint32 pageNumber);
  bool isEndPage(qint32 pageNumber);
  bool isStartPage(qint32 pageNumber);

private slots:
  void synchronizeNowButtonClicked();
  void searchTextChanged(const QString &text);
  void nextPageButtonClicked();
  void prevPageButtonClicked();
  void addButtonClicked();
  void bookInsertedHandle();
  void saveChanges();
  void searchFilterAccepted();
  void deleteButtonClicked();

private:
  Ui::BookSection *ui;

  BookAddDialog *m_bookAddDialog;
  SearchFilterDialog *m_searchFilterDialog;
  BookSectionDAO *m_dao;
  BookDetailsDialog *m_bookDetails;

  bool m_pageLoading;

  qint32 m_currentPage;
  qint32 m_booksCount;
};

#endif  // BOOKSECTION_H
