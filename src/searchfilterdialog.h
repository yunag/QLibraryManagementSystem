#ifndef SEARCHFILTERDIALOG_H
#define SEARCHFILTERDIALOG_H

#include <QDialog>

#include "booksectiondao.h"

namespace Ui {
class SearchFilterDialog;
}

class BookSectionDAO;

class SearchFilterDialog : public QDialog {
  Q_OBJECT

public:
  explicit SearchFilterDialog(BookSectionDAO *dao, QWidget *parent = nullptr);
  ~SearchFilterDialog();

public:
  void open() override;
  void accept() override;

private slots:
  void indexChanged(int index);

private:
  void setOrder(Qt::SortOrder order);
  Qt::SortOrder order() const;

private:
  Ui::SearchFilterDialog *ui;
  BookSectionDAO *m_dao;

  Qt::SortOrder m_order;
  BookSectionDAO::Column m_column;
};

#endif  // SEARCHFILTERDIALOG_H
