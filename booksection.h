#ifndef BOOKSECTION_H
#define BOOKSECTION_H

#include <QFuture>
#include <QListWidgetItem>
#include <QSqlQuery>
#include <QWidget>

#include "bookadddialog.h"
#include "bookcard.h"
#include "booksectiondao.h"

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

  bool isEndPage(qint32 pageNumber);
  bool isStartPage(qint32 pageNumber);

private slots:
  void synchronizeNowButtonClicked();
  void searchTextReturnPressed();
  void nextPageButtonClicked();
  void prevPageButtonClicked();
  void addButtonClicked();

private:
  Ui::BookSection *ui;

  BookCard *m_bookCards[kItemsPerPage];
  QListWidgetItem *m_bookItems[kItemsPerPage];
  BookAddDialog *m_bookAddDialog;
  BookSectionDAO m_DAO;

  bool m_pageLoading;

  qint32 m_currentPage;
  quint32 m_booksCount;
};

#endif  // BOOKSECTION_H
