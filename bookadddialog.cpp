#include <QMessageBox>
#include <QSqlError>
#include <QStandardItem>

#include "book.h"
#include "bookadddialog.h"
#include "bookauthor.h"
#include "bookcategory.h"
#include "librarydatabase.h"
#include "ui_bookadddialog.h"

BookAddDialog::BookAddDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BookAddDialog) {
  ui->setupUi(this);

  QPixmap pixmap(":/resources/images/DefaultBookCover.jpg");
  ui->coverLabel->setPixmap(pixmap);
  ui->coverLabel->setAspectRatio(Qt::KeepAspectRatio);

  BookCategory::categories().then(
    this, [this](const QList<BookCategory> &categories) {
      QList<QStandardItem *> items;

      for (const BookCategory &category : categories) {
        QStandardItem *item = new QStandardItem(category.name);
        item->setData(category.id);

        items.append(item);
      }

      ui->categories->addItems(items);
    });

  BookAuthor::authors().then(this, [this](const QList<BookAuthor> &authors) {
    QList<QStandardItem *> items;

    for (const BookAuthor &author : authors) {
      QStandardItem *item =
        new QStandardItem(author.firstName + " " + author.lastName);
      item->setData(author.id);

      items.append(item);
    }

    ui->authors->addItems(items);
  });

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &BookAddDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &BookAddDialog::reject);
}

BookAddDialog::~BookAddDialog() {
  delete ui;
}

void BookAddDialog::accept() {
  if (ui->categories->resultList().isEmpty() ||
      ui->authors->resultList().isEmpty() ||
      ui->titleLineEdit->text().isEmpty()) {
    return;
  }

  QString title = ui->titleLineEdit->text();
  QString publicationDate = ui->dateEdit->date().toString(Qt::ISODate);
  QString coverPath = ui->coverLabel->imagePath();

  Book book(title, publicationDate, coverPath, 0);

  Book::insert(book)
    .then(QtFuture::Launch::Async,
          [this](quint32 book_id) {
            QList<QStandardItem *> authors = ui->authors->resultList();

            QList<quint32> author_id;
            for (QStandardItem *author : authors) {
              author_id << author->data().toUInt();
            }

            qDebug() << "update" << QThread::currentThread();
            BookAuthor::update(book_id, author_id).waitForFinished();
            qDebug() << "update end";

            return book_id;
          })
    .then(QtFuture::Launch::Async,
          [this](quint32 book_id) {
            QList<QStandardItem *> categories = ui->categories->resultList();

            QList<quint32> category_id;
            for (QStandardItem *category : categories) {
              category_id << category->data().toUInt();
            }

            BookCategory::update(book_id, category_id).waitForFinished();

            emit inserted(book_id);
          })
    .onFailed(this,
              [this](const QSqlError &e) { databaseErrorMessageBox(this, e); });

  QDialog::accept();
}
