#include <QStandardItem>
#include <QStandardItemModel>

#include "authordetailsdialog.h"
#include "smoothscrollbar.h"
#include "ui_authordetailsdialog.h"

#include "common/error.h"

#include "network/networkerror.h"

#include "dao/authordetailsdao.h"

AuthorDetailsDialog::AuthorDetailsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AuthorDetailsDialog),
      m_dao(new AuthorDetailsDAO(this)) {
  ui->setupUi(this);

  auto *model = new QStandardItemModel(ui->booksList);
  ui->booksList->setModel(model);

  ui->authorImage->setStyleSheet("");
  ui->authorImage->setAspectRatio(Qt::KeepAspectRatio);

  ui->booksList->setIconSize(kItemSize * 0.8);

  auto *smoothScrollBar = new SmoothScrollBar(ui->booksList);
  ui->booksList->setAcceptDrops(false);
  ui->booksList->verticalScrollBar()->setDisabled(true);
  ui->booksList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->booksList->setHorizontalScrollBar(smoothScrollBar);

  ui->booksList->setDragDropMode(QAbstractItemView::NoDragDrop);

  ui->booksList->setMaximumHeight(
    kItemSize.height() + ui->booksList->horizontalScrollBar()->height());

  connect(ui->booksList, &QListView::doubleClicked, this,
          [this, model](const QModelIndex &index) {
            QStandardItem *item = model->item(index.row());

            emit bookDetailsRequested(item->data().toUInt());
          });
}

AuthorDetailsDialog::~AuthorDetailsDialog() {
  delete ui;
}

void AuthorDetailsDialog::openDetails(quint32 authorId) {

  m_dao->fetchDetails(authorId)
    .then(this,
          [this](const AuthorDetails &details) {
            updateUi(details);
            open();
          })
    .onFailed(this,
              [this](const NetworkError &err) { handleError(this, err); });
}

void AuthorDetailsDialog::showDetails(quint32 authorId) {
  m_dao->fetchDetails(authorId)
    .then(this,
          [this](const AuthorDetails &details) {
            updateUi(details);
            show();
          })
    .onFailed(this,
              [this](const NetworkError &err) { handleError(this, err); });
}

void AuthorDetailsDialog::updateUi(const AuthorDetails &) {
  /* TODO: implement */
}
