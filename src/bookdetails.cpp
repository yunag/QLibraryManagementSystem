#include <QDate>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include <QFontMetrics>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QStyledItemDelegate>

#include <QSqlError>

#include "bookdetails.h"

#include "database/bookdetailsdao.h"
#include "database/librarydatabase.h"
#include "forms/ui_bookdetails.h"

BookDetailsDialog::BookDetailsDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BookDetailsDialog) {

  ui->setupUi(this);
  ui->scrollArea->verticalScrollBar()->setSingleStep(8);

  m_dao = new BookDetailsDAO(this);
  QStandardItemModel *authorsModel = new QStandardItemModel(ui->authorsList);
  ui->authorsList->setModel(authorsModel);

  QStandardItemModel *categoriesModel =
    new QStandardItemModel(ui->categoriesList);
  ui->categoriesList->setModel(categoriesModel);

  setupList(ui->authorsList);
  setupList(ui->categoriesList);
}

BookDetailsDialog::~BookDetailsDialog() {
  delete ui;
}

void BookDetailsDialog::openBook(quint32 bookId) {
  m_dao->fetchDetails(bookId)
    .then(
      this,
      [this, bookId](const BookDetails &bookDetails) {
        Book book = bookDetails.book;

        ui->bookTitleLabel->setText(book.title);
        ui->descriptionText->setText(book.description);
        ui->bookDateLabel->setText(book.publication_date.toString());
        ui->numCopiesOwnedLabel->setText(QString::number(book.copies_owned));
        ui->bookIdLabel->setText(QString::number(bookId));

        QStandardItemModel *authorsModel =
          qobject_cast<QStandardItemModel *>(ui->authorsList->model());

        for (const Author &author : bookDetails.authors) {
          QString authorFullName = author.first_name + " " + author.last_name;
          QStandardItem *item = new QStandardItem(
            QIcon(":/resources/images/DefaultBookCover"), authorFullName);

          item->setFlags(item->flags() & ~Qt::ItemIsEditable);
          item->setData(author.author_id);
          item->setTextAlignment(Qt::AlignBaseline | Qt::AlignHCenter);
          item->setSizeHint(kItemSize);
          authorsModel->appendRow(item);
        }

        QStandardItemModel *categoriesModel =
          qobject_cast<QStandardItemModel *>(ui->categoriesList->model());

        for (const Category &category : bookDetails.categories) {
          QStandardItem *item = new QStandardItem(
            QIcon(":/resources/images/DefaultBookCover"), category.name);

          item->setFlags(item->flags() & ~Qt::ItemIsEditable);
          item->setData(category.category_id);
          item->setTextAlignment(Qt::AlignBaseline | Qt::AlignHCenter);
          item->setSizeHint(kItemSize);
          categoriesModel->appendRow(item);
        }

        QDialog::open();
      })
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);
    });
}

void BookDetailsDialog::setupList(QListView *listView) {

  listView->setIconSize(kItemSize * 0.8f);

  listView->setAcceptDrops(false);
  listView->verticalScrollBar()->setDisabled(true);
  listView->setDragDropMode(QAbstractItemView::NoDragDrop);
  listView->horizontalScrollBar()->setSingleStep(8);

  listView->setMaximumHeight(kItemSize.height() +
                             listView->horizontalScrollBar()->height());
}
