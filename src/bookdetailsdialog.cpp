#include <QDate>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QFontMetrics>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

#include <QSqlError>

#include "bookdetailsdialog.h"
#include "imageloader.h"
#include "smoothscrollbar.h"

#include "database/bookdetailsdao.h"
#include "database/librarydatabase.h"
#include "ui_bookdetailsdialog.h"

BookDetailsDialog::BookDetailsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BookDetailsDialog) {

  ui->setupUi(this);

  m_dao = new BookDetailsDAO(this);
  QStandardItemModel *authorsModel = new QStandardItemModel(ui->authorsList);
  ui->authorsList->setModel(authorsModel);

  QStandardItemModel *categoriesModel =
    new QStandardItemModel(ui->categoriesList);
  ui->categoriesList->setModel(categoriesModel);

  ui->bookCover->setStyleSheet("");
  ui->bookCover->setAspectRatio(Qt::KeepAspectRatio);

  setupList(ui->authorsList);
  setupList(ui->categoriesList);

  connect(ui->authorsList, &QListView::doubleClicked, this,
          [this](const QModelIndex &index) {
            QStandardItemModel *model =
              qobject_cast<QStandardItemModel *>(ui->authorsList->model());
            QStandardItem *item = model->item(index.row());

            emit authorDetailsRequested(item->data().toUInt());
          });
}

BookDetailsDialog::~BookDetailsDialog() {
  delete ui;
}

void BookDetailsDialog::openDetails(quint32 bookId) {
  m_dao->fetchDetails(bookId)
    .then(this,
          [this](const BookDetails &bookDetails) {
            updateUi(bookDetails);
            open();
          })
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);
    });
}

void BookDetailsDialog::showDetails(quint32 bookId) {
  m_dao->fetchDetails(bookId)
    .then(this,
          [this](const BookDetails &bookDetails) {
            updateUi(bookDetails);
            show();
          })
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);
    });
}

void BookDetailsDialog::setupList(QListView *listView) {

  listView->setIconSize(kItemSize * 0.8f);

  SmoothScrollBar *smoothScrollBar = new SmoothScrollBar(listView);
  listView->setAcceptDrops(false);
  listView->verticalScrollBar()->setDisabled(true);
  listView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  listView->setHorizontalScrollBar(smoothScrollBar);

  listView->setDragDropMode(QAbstractItemView::NoDragDrop);

  listView->setMaximumHeight(kItemSize.height() +
                             listView->horizontalScrollBar()->height());
}

QStandardItem *BookDetailsDialog::addItem(QStandardItemModel *model,
                                          const QIcon &icon,
                                          const QString &text) {
  QStandardItem *item = new QStandardItem(icon, text);

  item->setFlags(item->flags() & ~Qt::ItemIsEditable);
  item->setTextAlignment(Qt::AlignBaseline | Qt::AlignHCenter);
  item->setSizeHint(kItemSize);
  model->appendRow(item);

  return item;
}

void BookDetailsDialog::updateUi(const BookDetails &bookDetails) {
  Book book = bookDetails.book;

  ui->bookTitleLabel->setText(book.title);
  ui->descriptionText->setText(book.description);
  ui->bookDateLabel->setText(book.publication_date.toString());
  ui->numCopiesOwnedLabel->setText(QString::number(book.copies_owned));
  ui->bookIdLabel->setText(QString::number(book.book_id));

  ui->bookCover->setPixmap(ImageLoader::load(book.cover_path));

  QStandardItemModel *authorsModel =
    qobject_cast<QStandardItemModel *>(ui->authorsList->model());
  authorsModel->clear();

  for (const Author &author : bookDetails.authors) {
    QString authorFullName = author.first_name + " " + author.last_name;
    QStandardItem *item =
      addItem(authorsModel, ImageLoader::load(""), authorFullName);

    item->setData(author.author_id);
  }

  QStandardItemModel *categoriesModel =
    qobject_cast<QStandardItemModel *>(ui->categoriesList->model());
  categoriesModel->clear();

  for (const Category &category : bookDetails.categories) {
    QStandardItem *item =
      addItem(categoriesModel, ImageLoader::load(""), category.name);

    item->setData(category.category_id);
  }
}
