#include <QFontMetrics>
#include <QStandardItemModel>

#include "controllers/bookcontroller.h"
#include "loadingmodel.h"

#include "common/error.h"
#include "common/widgetutils.h"

#include "bookdetailsdialog.h"
#include "libraryapplication.h"
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
            auto *model =
              qobject_cast<QStandardItemModel *>(ui->authorsList->model());
            QStandardItem *item = model->item(index.row());

            emit authorDetailsRequested(item->data().toUInt());
          });
}

BookDetailsDialog::~BookDetailsDialog() {
  delete ui;
}

void BookDetailsDialog::showDetails(quint32 bookId) {
  BookController::getBookById(bookId)
    .then(this,
          [this](const BookData &bookDetails) {
            updateUi(bookDetails);
            show();
          })
    .onFailed(this,
              [this](const NetworkError &err) { handleError(this, err); });
}

void BookDetailsDialog::setupList(QListView *listView) {

  listView->setIconSize(kItemSize * 0.8);

  auto *smoothScrollBar = new SmoothScrollBar(listView);
  listView->setAcceptDrops(false);
  listView->verticalScrollBar()->setDisabled(true);
  listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  listView->setHorizontalScrollBar(smoothScrollBar);

  listView->setDragDropMode(QAbstractItemView::NoDragDrop);

  listView->setMaximumHeight(kItemSize.height() +
                             listView->horizontalScrollBar()->height());
}

QStandardItem *BookDetailsDialog::addItem(LoadingModel *model, const QUrl &url,
                                          const QString &text) {
  auto *item = new QStandardItem(text);
  auto movie = App->busyIndicator();

  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  item->setTextAlignment(Qt::AlignBaseline | Qt::AlignHCenter);
  item->setSizeHint(kItemSize);
  model->appendRow(item, url, movie);

  return item;
}

static QUrl g_testUrl =
  QUrl("https://external-preview.redd.it/"
       "0ribEcoh7Jr0AMq9GuUg7ZNFvDARr_ltk2O25GFd_Go.jpg?auto=webp&s="
       "8ff7fd9d3a667f646a8dd9673293084309fd8d85");

void BookDetailsDialog::updateUi(const BookData &book) {
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

    QStandardItem *item = addItem(m_authorsModel, g_testUrl, authorFullName);

    item->setData(author.id);
  }

  m_categoriesModel->clear();

  for (const Category &category : book.categories) {
    QStandardItem *item = addItem(m_categoriesModel, g_testUrl, category.name);

    item->setData(category.id);
  }
}

void BookDetailsDialog::closeEvent(QCloseEvent *) {
  m_authorsModel->clear();
  m_categoriesModel->clear();

  QDialog::close();
}
