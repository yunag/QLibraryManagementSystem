#ifndef SEARCHFILTERDIALOG_H
#define SEARCHFILTERDIALOG_H

#include <QDialog>

#include "dao/booksectiondao.h"

namespace Ui {
class SearchFilterDialog;
}

class SearchFilterDialog : public QDialog {
  Q_OBJECT

public:
  explicit SearchFilterDialog(BookSectionDao *dao, QWidget *parent = nullptr);
  ~SearchFilterDialog() override;

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
  BookSectionDao *m_dao;

  Qt::SortOrder m_order;
  BookSectionDao::Property m_column;
};

#endif  // SEARCHFILTERDIALOG_H
