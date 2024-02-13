#ifndef TWOWAYLIST_H
#define TWOWAYLIST_H

#include <QWidget>

namespace Ui {
class TwoWayList;
}

class QSortFilterProxyModel;
class QListWidgetItem;
class QStandardItemModel;
class QStandardItem;

class TwoWayList : public QWidget {
  Q_OBJECT

public:
  explicit TwoWayList(QWidget *parent = nullptr);
  ~TwoWayList();

  void addItems(QList<QStandardItem *> items);
  QList<QStandardItem *> resultList();
  bool hasResults();

private slots:
  void returnPressed();
  void swapFromLeftToRight(const QModelIndex &index);
  void swapFromRightToLeft(const QModelIndex &index);

private:
  Ui::TwoWayList *ui;

  QSortFilterProxyModel *m_proxy;
  QStandardItemModel *m_modelLeft;
  QStandardItemModel *m_modelRight;
};

#endif  // TWOWAYLIST_H
