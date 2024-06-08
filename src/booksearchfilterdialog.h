#ifndef BOOKSEARCHFILTERDIALOG_H
#define BOOKSEARCHFILTERDIALOG_H

#include <QDialog>

class BookRestModel;

namespace Ui {
class BookSearchFilterDialog;
}

class BookSearchFilterDialog : public QDialog {
  Q_OBJECT

public:
  explicit BookSearchFilterDialog(BookRestModel *model,
                                  QWidget *parent = nullptr);
  ~BookSearchFilterDialog() override;

public:
  void accept() override;

private:
  Ui::BookSearchFilterDialog *ui;
  BookRestModel *m_model;
};

#endif  // BOOKSEARCHFILTERDIALOG_H
