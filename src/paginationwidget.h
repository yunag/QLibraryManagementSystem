#ifndef PAGINATIONWIDGET_H
#define PAGINATIONWIDGET_H

#include <QWidget>

#include "pagination.h"

class QIntValidator;

namespace Ui {
class PaginationWidget;
}

class PaginationWidget : public QWidget {
  Q_OBJECT

public:
  explicit PaginationWidget(QWidget *parent = nullptr);
  ~PaginationWidget() override;

  void setPagination(Pagination *pagination);
  Pagination *pagination() const;

  bool isNextPageAvailable() const;
  bool isPrevPageAvailable() const;
  bool isPageValid(int page) const;

public slots:
  void nextPage();
  void prevPage();

protected:
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void currentPageChanged(int page);
  void inputEdited(const QString &text);

private:
  void updatePageButtons();
  void updateTotalPages();

private:
  Ui::PaginationWidget *ui;
  QIntValidator *m_pageValidator;

  Pagination *m_pagination;
};

#endif  // PAGINATIONWIDGET_H
