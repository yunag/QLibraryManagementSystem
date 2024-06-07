#ifndef AUTHORADDDIALOG_H
#define AUTHORADDDIALOG_H

#include <QDialog>

namespace Ui {
class AuthorAddDialog;
}

class AuthorAddDialog : public QDialog {
  Q_OBJECT

public:
  explicit AuthorAddDialog(QWidget *parent = nullptr);
  ~AuthorAddDialog() override;

  void createAuthor();

  void init();

private:
  bool isUpdating;

  Ui::AuthorAddDialog *ui;
};

#endif  // AUTHORADDDIALOG_H
