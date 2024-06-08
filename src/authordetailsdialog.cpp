#include "authordetailsdialog.h"
#include "ui_authordetailsdialog.h"

#include "common/error.h"
#include "common/widgetutils.h"

#include "controllers/authorcontroller.h"

#include "model/loadingmodel.h"
#include "schema/schema.h"

#include "smoothscrollbar.h"

AuthorDetailsDialog::AuthorDetailsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AuthorDetailsDialog),
      m_bookModel(new LoadingModel(this)) {
  ui->setupUi(this);

  ui->booksList->setModel(m_bookModel);
  ui->booksList->setTextElideMode(Qt::ElideRight);

  ui->authorImage->setStyleSheet("");
  ui->authorImage->setAspectRatio(Qt::KeepAspectRatio);

  ui->booksList->setIconSize(kItemSize * 0.78);

  auto *smoothScrollBar = new SmoothScrollBar(ui->booksList);
  ui->booksList->setAcceptDrops(false);
  ui->booksList->verticalScrollBar()->setDisabled(true);
  ui->booksList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->booksList->setHorizontalScrollBar(smoothScrollBar);

  ui->booksList->setDragDropMode(QAbstractItemView::NoDragDrop);

  connect(ui->booksList, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
    QStandardItem *item = m_bookModel->item(index.row());

    emit bookDetailsRequested(item->data().toUInt());
  });
}

AuthorDetailsDialog::~AuthorDetailsDialog() {
  delete ui;
}

void AuthorDetailsDialog::showDetails(quint32 authorId) {
  AuthorController controller;

  controller.get(authorId)
    .then(this, [this](const AuthorDetails &details) {
    updateUi(details);
    show();
  }).onFailed(this, [this](const NetworkError &err) {
    handleError(this, err);
  });
}

void AuthorDetailsDialog::updateUi(const AuthorDetails &details) {
  ui->authorNameLabel->setText(details.firstName + " " + details.lastName);
  /* TODO: implement */
  // ui->bioText->setText(details.bio);
  WidgetUtils::asyncLoadImage(ui->authorImage, details.imageUrl);

  m_bookModel->clear();

  for (const Book &book : details.books) {
    QStandardItem *item = addItem(m_bookModel, book.coverUrl, book.title);

    item->setData(book.id);
  }
}

QStandardItem *AuthorDetailsDialog::addItem(LoadingModel *model,
                                            const QUrl &url,
                                            const QString &text) {
  auto *item = new QStandardItem(text);

  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  item->setTextAlignment(Qt::AlignBaseline | Qt::AlignHCenter);
  item->setSizeHint(kItemSize);
  model->appendRow(item, url);

  return item;
}

void AuthorDetailsDialog::closeEvent(QCloseEvent *event) {
  m_bookModel->clear();

  QDialog::close();
}
