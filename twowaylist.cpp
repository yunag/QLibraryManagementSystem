#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStringListModel>

#include "twowaylist.h"
#include "ui_twowaylist.h"

TwoWayList::TwoWayList(QWidget *parent)
    : QWidget(parent), ui(new Ui::TwoWayList) {
  ui->setupUi(this);

  m_modelLeft = new QStandardItemModel(this);
  m_proxy = new QSortFilterProxyModel(this);

  m_modelRight = new QStandardItemModel(this);

  m_proxy->setSourceModel(m_modelLeft);
  m_proxy->setFilterCaseSensitivity(Qt::CaseInsensitive);

  ui->leftListView->setModel(m_proxy);
  ui->rightListView->setModel(m_modelRight);

  ui->leftListView->acceptDropsFrom(ui->rightListView);
  ui->rightListView->acceptDropsFrom(ui->leftListView);

  connect(ui->lineEdit, &QLineEdit::textChanged, m_proxy,
          &QSortFilterProxyModel::setFilterFixedString);
  connect(ui->lineEdit, &QLineEdit::returnPressed, this,
          &TwoWayList::returnPressed);

  connect(ui->leftListView, &QListView::doubleClicked, this,
          &TwoWayList::swapFromLeftToRight);
  connect(ui->rightListView, &QListView::doubleClicked, this,
          &TwoWayList::swapFromRightToLeft);
}

TwoWayList::~TwoWayList() {
  delete ui;
}

void TwoWayList::addItems(QList<QStandardItem *> items) {
  for (QStandardItem *item : items) {
    item->setFlags(item->flags() & ~Qt::ItemIsDropEnabled);
    m_modelLeft->appendRow(item);
  }
}

void TwoWayList::returnPressed() {
  if (m_proxy->rowCount()) {
    swapFromLeftToRight(m_proxy->index(0, 0));
  }
}

void TwoWayList::swapFromLeftToRight(const QModelIndex &index) {
  QModelIndex sourceIndex = m_proxy->mapToSource(index);

  QList<QStandardItem *> item = m_modelLeft->takeRow(sourceIndex.row());

  m_modelRight->appendRow(item);
}

void TwoWayList::swapFromRightToLeft(const QModelIndex &index) {
  QList<QStandardItem *> item = m_modelRight->takeRow(index.row());

  m_modelLeft->appendRow(item);
}

QList<QStandardItem *> TwoWayList::resultList() {
  QList<QStandardItem *> result;

  for (int i = 0; i < m_modelRight->rowCount(); ++i) {
    result << m_modelRight->item(i);
  }
  return result;
}

bool TwoWayList::hasResults() {
  return m_modelRight->rowCount();
}
