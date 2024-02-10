#ifndef BOOKSECTION_H
#define BOOKSECTION_H

#include <QFuture>
#include <QWidget>

class BookAddDialog;
class BookSectionDAO;

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

private:
  Ui::BookSection *ui;

  BookAddDialog *m_bookAddDialog;
  BookSectionDAO *m_dao;

  bool m_pageLoading;

  qint32 m_currentPage;
  qint32 m_booksCount;
};

#endif  // BOOKSECTION_H
