#ifndef BOOKADDDIALOG_H
#define BOOKADDDIALOG_H

#include <QDialog>

namespace Ui {
class BookAddDialog;
}

class BookAddDialog : public QDialog {
  Q_OBJECT

public:
  explicit BookAddDialog(QWidget *parent = nullptr);
  ~BookAddDialog();

  void accept();

private:
  Ui::BookAddDialog *ui;
};

#endif  // BOOKADDDIALOG_H
