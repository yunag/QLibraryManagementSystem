#ifndef BOOKDETAILSDIALOG_H
#define BOOKDETAILSDIALOG_H

#include <QDialog>

#include "schema/schema.h"

class QListView;
class QStandardItemModel;
class QStandardItem;
class LoadingModel;

namespace Ui {
class BookDetailsDialog;
}

class BookDetailsDialog : public QDialog {
  Q_OBJECT

  static constexpr QSize kItemSize = QSize(130, 200);

public:
  explicit BookDetailsDialog(QWidget *parent = nullptr);
  ~BookDetailsDialog() override;

  void showDetails(quint32 bookId);

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  void updateUi(const BookDetails &bookDetails);
  static void setupList(QListView *listView);
  static QStandardItem *addItem(LoadingModel *model, const QUrl &url,
                                const QString &text);

private:
  Ui::BookDetailsDialog *ui;
  LoadingModel *m_authorsModel;
  LoadingModel *m_categoriesModel;
};

#endif  // BOOKDETAILSDIALOG_H
