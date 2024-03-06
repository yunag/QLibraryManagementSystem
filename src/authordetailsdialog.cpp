#include <QSqlError>
#include <QStandardItem>
#include <QStandardItemModel>

#include "authordetailsdialog.h"
#include "imageloader.h"
#include "smoothscrollbar.h"
#include "ui_authordetailsdialog.h"

#include "database/authordetailsdao.h"
#include "database/librarydatabase.h"

AuthorDetailsDialog::AuthorDetailsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AuthorDetailsDialog) {
  ui->setupUi(this);

  m_dao = new AuthorDetailsDAO(this);

  QStandardItemModel *model = new QStandardItemModel(ui->booksList);
  ui->booksList->setModel(model);

  ui->authorImage->setStyleSheet("");
  ui->authorImage->setAspectRatio(Qt::KeepAspectRatio);

  ui->booksList->setIconSize(kItemSize * 0.8f);

  SmoothScrollBar *smoothScrollBar = new SmoothScrollBar(ui->booksList);
  ui->booksList->setAcceptDrops(false);
  ui->booksList->verticalScrollBar()->setDisabled(true);
  ui->booksList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->booksList->setHorizontalScrollBar(smoothScrollBar);

  ui->booksList->setDragDropMode(QAbstractItemView::NoDragDrop);

  ui->booksList->setMaximumHeight(
    kItemSize.height() + ui->booksList->horizontalScrollBar()->height());

  connect(ui->booksList, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
            QStandardItemModel *model =
              qobject_cast<QStandardItemModel *>(ui->booksList->model());
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
    .onFailed(
      [this](const QSqlError &err) { databaseErrorMessageBox(this, err); });
}

void AuthorDetailsDialog::showDetails(quint32 authorId) {
  m_dao->fetchDetails(authorId)
    .then(this,
          [this](const AuthorDetails &details) {
            updateUi(details);
            show();
          })
    .onFailed(
      [this](const QSqlError &err) { databaseErrorMessageBox(this, err); });
}

void AuthorDetailsDialog::updateUi(const AuthorDetails &details) {
  const Author &author = details.author;

  ui->authorNameLabel->setText(author.first_name + " " + author.last_name);
  ui->bioText->setText("Lorem Ipsum");
  ui->authorImage->setPixmap(ImageLoader::load(""));

  const QList<AuthorDetails::BookInfo> &books = details.books;
  QStandardItemModel *booksModel =
    qobject_cast<QStandardItemModel *>(ui->booksList->model());
  booksModel->clear();

  for (const auto &book : books) {
    QStandardItem *item =
      new QStandardItem(ImageLoader::load(book.coverPath), book.title);

    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    item->setTextAlignment(Qt::AlignBaseline | Qt::AlignHCenter);
    item->setSizeHint(kItemSize);
    item->setData(book.id);
    booksModel->appendRow(item);
  }
}
