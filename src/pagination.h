#ifndef PAGINATION_H
#define PAGINATION_H

#include <QFrame>

class QIntValidator;

namespace Ui {
class Pagination;
}

class Pagination : public QFrame {
  Q_OBJECT

public:
  explicit Pagination(QWidget *parent = nullptr);
  Pagination(int itemsCount, int itemsPerPageCount, QWidget *parent = nullptr);

  void setItemsCount(int newItemsCount);
  int itemsCount() const;

  void setItemsPerPageCount(int newItemsPerPageCount);
  int itemsPerPageCount() const;

  void setPage(int newPage);

  int currentPage() const;
  void setCurrentPage(int newPage);

  bool isNextPageAvailable();
  bool isPrevPageAvailable();

  bool isLastPage();
  bool isFirstPage();

  bool isPageValid(int page);

  void nextPage();
  void prevPage();

  void firstPage();
  void lastPage();

signals:
  void pageChanged(int page);

private:
  void updateTotalPages();

  int calculateTotalPages();
  void updatePageButtons();

private:
  Ui::Pagination *ui;
  QIntValidator *m_pageValidator;

  int m_itemsCount;
  int m_currentPage;
  int m_itemsPerPageCount;
  int m_totalPages;
};

#endif  // PAGINATION_H
