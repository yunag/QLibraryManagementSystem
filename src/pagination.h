#ifndef PAGINATION_H
#define PAGINATION_H

#include <QWidget>

class QIntValidator;

namespace Ui {
class Pagination;
}

class Pagination : public QWidget {
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

  bool isNextPageAvailable() const;
  bool isPrevPageAvailable() const;

  bool isLastPage() const;
  bool isFirstPage() const;

  bool isPageValid(int page) const;

  void nextPage();
  void prevPage();

  void firstPage();
  void lastPage();

signals:
  void pageChanged(int page);

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  void updateTotalPages();

  int calculateTotalPages() const;
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
