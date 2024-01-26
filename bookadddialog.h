#ifndef BOOKADDDIALOG_H
#define BOOKADDDIALOG_H

#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class BookAddDialog;
}

class BookAddDialog : public QWidget {
  Q_OBJECT

public:
  explicit BookAddDialog(QWidget *parent = nullptr);
  ~BookAddDialog();

private slots:
  void categoriesReturnPressed();
  void authorsReturnPressed();
  void authorsItemDoubleClicked(QListWidgetItem *item);
  void categoriesItemDoubleClicked(QListWidgetItem *item);

private:
  Ui::BookAddDialog *ui;
};

#endif  // BOOKADDDIALOG_H
