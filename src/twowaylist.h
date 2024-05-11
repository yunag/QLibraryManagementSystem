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
  ~TwoWayList() override;

  void addItems(const QList<QStandardItem *> &items);

  QList<QStandardItem *> rightList();
  QList<QStandardItem *> leftList();
  bool hasResults();

public slots:
  void swapLeftToRight(const QModelIndex &index);
  void swapRightToLeft(const QModelIndex &index);

private slots:
  void returnPressed();

private:
  Ui::TwoWayList *ui;

  QSortFilterProxyModel *m_proxy;
  QStandardItemModel *m_modelLeft;
  QStandardItemModel *m_modelRight;
};

#endif  // TWOWAYLIST_H
