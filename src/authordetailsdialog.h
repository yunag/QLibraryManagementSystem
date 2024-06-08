#ifndef AUTHORDETAILSDIALOG_H
#define AUTHORDETAILSDIALOG_H

#include <QDialog>

class AuthorDetails;
class LoadingModel;
class QStandardItem;

namespace Ui {
class AuthorDetailsDialog;
}

class AuthorDetailsDialog : public QDialog {
  Q_OBJECT

  static constexpr QSize kItemSize = QSize(130, 200);

public:
  explicit AuthorDetailsDialog(QWidget *parent = nullptr);
  ~AuthorDetailsDialog() override;

  void showDetails(quint32 authorId);

signals:
  void bookDetailsRequested(quint32 bookId);

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  void updateUi(const AuthorDetails &details);
  static QStandardItem *addItem(LoadingModel *model, const QUrl &url,
                                const QString &text);

private:
  Ui::AuthorDetailsDialog *ui;

  LoadingModel *m_bookModel;
};

#endif  // AUTHORDETAILSDIALOG_H
