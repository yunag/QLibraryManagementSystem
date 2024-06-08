#include <QFontMetrics>
#include <QStandardItemModel>

#include "controllers/bookcontroller.h"
#include "model/loadingmodel.h"

#include "common/error.h"
#include "common/widgetutils.h"

#include "bookdetailsdialog.h"
#include "ui_bookdetailsdialog.h"

#include "smoothscrollbar.h"

BookDetailsDialog::BookDetailsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BookDetailsDialog),
      m_authorsModel(new LoadingModel(this)),
      m_categoriesModel(new LoadingModel(this)) {
  ui->setupUi(this);

  ui->authorsList->setModel(m_authorsModel);
  ui->categoriesList->setModel(m_categoriesModel);

  ui->bookCover->setStyleSheet("");
  ui->bookCover->setAspectRatio(Qt::KeepAspectRatio);

  setupList(ui->authorsList);
  setupList(ui->categoriesList);

  connect(ui->authorsList, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
    auto *model = qobject_cast<QStandardItemModel *>(ui->authorsList->model());
    QStandardItem *item = model->item(index.row());

    emit authorDetailsRequested(item->data().toUInt());
  });
}

BookDetailsDialog::~BookDetailsDialog() {
  delete ui;
}

void BookDetailsDialog::showDetails(quint32 bookId) {
  BookController controller;

  controller.get(bookId)
    .then(this, [this](const BookDetails &bookDetails) {
    updateUi(bookDetails);
    show();
  }).onFailed(this, [this](const NetworkError &err) {
    handleError(this, err);
  });
}

void BookDetailsDialog::setupList(QListView *listView) {

  listView->setIconSize(kItemSize * 0.78);

  auto *smoothScrollBar = new SmoothScrollBar(listView);
  listView->setAcceptDrops(false);
  listView->verticalScrollBar()->setDisabled(true);
  listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  listView->setHorizontalScrollBar(smoothScrollBar);
  listView->setTextElideMode(Qt::ElideRight);

  listView->setDragDropMode(QAbstractItemView::NoDragDrop);
}

QStandardItem *BookDetailsDialog::addItem(LoadingModel *model, const QUrl &url,
                                          const QString &text) {
  auto *item = new QStandardItem(text);

  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  item->setTextAlignment(Qt::AlignBaseline | Qt::AlignHCenter);
  item->setSizeHint(kItemSize);
  model->appendRow(item, url);

  return item;
}

void BookDetailsDialog::updateUi(const BookDetails &book) {
  setWindowTitle(book.title);

  ui->bookTitleLabel->setText(book.title);
  ui->descriptionText->setText(book.description);
  ui->bookDateLabel->setText(book.publicationDate.toString(Qt::ISODate));
  ui->numCopiesOwnedLabel->setText(QString::number(book.copiesOwned));
  ui->bookIdLabel->setText(QString::number(book.id));

  WidgetUtils::asyncLoadImage(ui->bookCover, book.coverUrl);

  m_authorsModel->clear();

  for (const Author &author : book.authors) {
    QString authorFullName = author.firstName + " " + author.lastName;

    QStandardItem *item =
      addItem(m_authorsModel, author.imageUrl, authorFullName);

    item->setData(author.id);
  }

  m_categoriesModel->clear();

  for (const Category &category : book.categories) {
    QStandardItem *item = addItem(m_categoriesModel, QUrl(""), category.name);

    item->setData(category.id);
  }
}

void BookDetailsDialog::closeEvent(QCloseEvent *) {
  m_authorsModel->clear();
  m_categoriesModel->clear();

  QDialog::close();
}
