#include <QMessageBox>
#include <QSqlError>
#include <QStandardItem>

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

  LibraryDatabase::exec("SELECT category_id, name FROM category")
    .then(this, [this](LibraryTable table) {
      QList<QStandardItem *> items;

      for (const TableRow &row : table) {
        quint32 categoryId = row.data[0].toUInt();
        QString categoryName = row.data[1].toString();

        QStandardItem *item = new QStandardItem(categoryName);
        item->setData(categoryId);

        items.append(item);
      }

      ui->categories->addItems(items);
    });

  LibraryDatabase::exec(
    "SELECT author_id, concat(first_name, ' ', last_name) FROM author")
    .then(this, [this](LibraryTable table) {
      QList<QStandardItem *> items;

      for (const TableRow &row : table) {
        quint32 authorId = row.data[0].toUInt();
        QString authorName = row.data[1].toString();

        QStandardItem *item = new QStandardItem(authorName);
        item->setData(authorId);

        items.append(item);
      }

      ui->authors->addItems(items);
    });

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &BookAddDialog::accept);
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

  Book b(title, publicationDate, coverPath, 0);

  BookTable::insert(b)
    .then([this](quint32 book_id) {
      QList<QStandardItem *> authors = ui->authors->resultList();

      QList<quint32> author_id;
      for (QStandardItem *author : authors) {
        author_id << author->data().toUInt();
      }

      BookAuthor::update(book_id, author_id).waitForFinished();

      return book_id;
    })
    .then([this](quint32 book_id) {
      QList<QStandardItem *> categories = ui->categories->resultList();

      QList<quint32> category_id;
      for (QStandardItem *category : categories) {
        category_id << category->data().toUInt();
      }

      BookCategory::update(book_id, category_id).waitForFinished();
    })
    .onFailed(this,
              [this](const QSqlError &e) { databaseErrorMessageBox(this, e); });

  QDialog::accept();
}
