#ifndef SEARCHFILTERDIALOG_H
#define SEARCHFILTERDIALOG_H

#include <QDialog>

#include "bookrestmodel.h"

namespace Ui {
class SearchFilterDialog;
}

class SearchFilterDialog : public QDialog {
  Q_OBJECT

public:
  explicit SearchFilterDialog(BookRestModel *model, QWidget *parent = nullptr);
  ~SearchFilterDialog() override;

public:
  void open() override;
  void accept() override;

private:
  Ui::SearchFilterDialog *ui;
  BookRestModel *m_model;
};

#endif  // SEARCHFILTERDIALOG_H
