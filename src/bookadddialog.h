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

  void open() override;
  void accept() override;
  void updateAuthors();
  void updateCategories();

signals:
  void inserted(quint32 book_id);

private:
  Ui::BookAddDialog *ui;
};

#endif  // BOOKADDDIALOG_H
