#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStringListModel>

#include "twowaylist.h"
#include "ui_twowaylist.h"

TwoWayList::TwoWayList(QWidget *parent)
    : QWidget(parent), ui(new Ui::TwoWayList),
      m_proxy(new QSortFilterProxyModel(this)),
      m_modelLeft(new QStandardItemModel(this)),
      m_modelRight(new QStandardItemModel(this)) {
  ui->setupUi(this);

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
          &TwoWayList::swapLeftToRight);
  connect(ui->rightListView, &QListView::doubleClicked, this,
          &TwoWayList::swapRightToLeft);
}

TwoWayList::~TwoWayList() {
  delete ui;
}

void TwoWayList::addItems(const QList<QStandardItem *> &items) {
  for (QStandardItem *item : items) {
    item->setFlags(item->flags() & ~Qt::ItemIsDropEnabled);
    m_modelLeft->appendRow(item);
  }
}

void TwoWayList::returnPressed() {
  if (m_proxy->rowCount()) {
    swapLeftToRight(m_proxy->index(0, 0));
  }
}

void TwoWayList::swapLeftToRight(const QModelIndex &index) {
  QModelIndex sourceIndex =
    index.model() == m_proxy ? m_proxy->mapToSource(index) : index;

  QList<QStandardItem *> item = m_modelLeft->takeRow(sourceIndex.row());

  m_modelRight->appendRow(item);
}

void TwoWayList::swapRightToLeft(const QModelIndex &index) {
  QList<QStandardItem *> item = m_modelRight->takeRow(index.row());

  m_modelLeft->appendRow(item);
}

static QList<QStandardItem *> grabItems(QStandardItemModel *model) {
  QList<QStandardItem *> result;

  for (int i = 0; i < model->rowCount(); ++i) {
    result << model->item(i);
  }
  return result;
}

QList<QStandardItem *> TwoWayList::leftList() {
  return grabItems(m_modelLeft);
}

QList<QStandardItem *> TwoWayList::rightList() {
  return grabItems(m_modelRight);
}

bool TwoWayList::hasResults() {
  return m_modelRight->rowCount();
}
