#ifndef BOOKDETAILSDIALOG_H
#define BOOKDETAILSDIALOG_H

#include <QDialog>

class BookDetailsDAO;
class QListView;

namespace Ui {
class BookDetailsDialog;
}

class BookDetailsDialog : public QDialog {
  Q_OBJECT

  static constexpr QSize kItemSize = QSize(120, 150);

public:
  explicit BookDetailsDialog(QWidget *parent = nullptr);
  ~BookDetailsDialog();

  void openBook(quint32 bookId);

private:
  void setupList(QListView *listView);

private:
  Ui::BookDetailsDialog *ui;
  BookDetailsDAO *m_dao;
};

#endif  // BOOKDETAILSDIALOG_H
