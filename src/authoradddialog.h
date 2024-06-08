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
  void editAuthor(quint32 id);

  void accept() override;
  void init();

signals:
  void edited(quint32 id);

private:
  bool m_isUpdating;
  quint32 m_id;

  Ui::AuthorAddDialog *ui;
};

#endif  // AUTHORADDDIALOG_H
