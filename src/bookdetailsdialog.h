#ifndef BOOKDETAILSDIALOG_H
#define BOOKDETAILSDIALOG_H

#include <QDialog>

class BookDetailsDAO;
class QListView;
class QStandardItemModel;
class QStandardItem;
class BookDetails;

namespace Ui {
class BookDetailsDialog;
}

class BookDetailsDialog : public QDialog {
  Q_OBJECT

  static constexpr QSize kItemSize = QSize(120, 150);

public:
  explicit BookDetailsDialog(QWidget *parent = nullptr);
  ~BookDetailsDialog();

  void openDetails(quint32 bookId);
  void showDetails(quint32 bookId);

signals:
  void authorDetailsRequested(quint32 authorId);

private:
  void updateUi(const BookDetails &bookDetails);
  void setupList(QListView *listView);
  QStandardItem *addItem(QStandardItemModel *model, const QIcon &icon,
                         const QString &text);

private:
  Ui::BookDetailsDialog *ui;
  BookDetailsDAO *m_dao;
};

#endif  // BOOKDETAILSDIALOG_H
