#ifndef AUTHORDETAILSDIALOG_H
#define AUTHORDETAILSDIALOG_H

#include <QDialog>

class AuthorDetailsDAO;
class AuthorDetails;

namespace Ui {
class AuthorDetailsDialog;
}

class AuthorDetailsDialog : public QDialog {
  Q_OBJECT

  static constexpr QSize kItemSize = QSize(120, 150);

public:
  explicit AuthorDetailsDialog(QWidget *parent = nullptr);
  ~AuthorDetailsDialog();

  void openDetails(quint32 authorId);
  void showDetails(quint32 authorId);
signals:
  void bookDetailsRequested(quint32 bookId);

private:
  void updateUi(const AuthorDetails &details);

  Ui::AuthorDetailsDialog *ui;
  AuthorDetailsDAO *m_dao;
};

#endif  // AUTHORDETAILSDIALOG_H
