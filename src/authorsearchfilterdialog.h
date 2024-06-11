#ifndef AUTHORSEARCHFILTERDIALOG_H
#define AUTHORSEARCHFILTERDIALOG_H

#include <QDialog>

class AuthorRestModel;

namespace Ui {
class AuthorSearchFilterDialog;
}

class AuthorSearchFilterDialog : public QDialog {
  Q_OBJECT

public:
  explicit AuthorSearchFilterDialog(AuthorRestModel *model,
                                    QWidget *parent = nullptr);
  ~AuthorSearchFilterDialog() override;

  void setSearchText(const QString &text);

public:
  void accept() override;

private:
  Ui::AuthorSearchFilterDialog *ui;
  AuthorRestModel *m_model;
};

#endif  // AUTHORSEARCHFILTERDIALOG_H
