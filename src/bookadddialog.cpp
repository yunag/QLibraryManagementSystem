#include <QMessageBox>
#include <QSqlError>
#include <QStandardItem>

#include "bookadddialog.h"
#include "database/author.h"
#include "database/book.h"
#include "database/bookauthor.h"
#include "database/bookcategory.h"
#include "database/category.h"
#include "database/librarydatabase.h"
#include "forms/ui_bookadddialog.h"

BookAddDialog::BookAddDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::BookAddDialog) {
  ui->setupUi(this);

  QPixmap pixmap(":/resources/images/DefaultBookCover");
  ui->coverLabel->setPixmap(pixmap);
  ui->coverLabel->setAspectRatio(Qt::KeepAspectRatio);

  LibraryDatabase::getEntries<Category>().then(
    this, [this](const QList<Category> &categories) {
      QList<QStandardItem *> items;

      for (const Category &category : categories) {
        QStandardItem *item = new QStandardItem(category.name);
        item->setData(category.category_id);

        items.append(item);
      }

      ui->categories->addItems(items);
    });

  LibraryDatabase::getEntries<Author>().then(
    this, [this](const QList<Author> &authors) {
      QList<QStandardItem *> items;

      for (const Author &author : authors) {
        QStandardItem *item =
          new QStandardItem(author.first_name + " " + author.last_name);
        item->setData(author.author_id);

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

static QList<quint32> getIds(QList<QStandardItem *> items) {
  QList<quint32> author_id;
  for (QStandardItem *item : items) {
    author_id << item->data().toUInt();
  }

  return author_id;
}

void BookAddDialog::accept() {
  if (ui->titleLineEdit->text().isEmpty()) {
    return;
  }

  QList<quint32> author_ids = getIds(ui->authors->resultList());
  QList<quint32> category_ids = getIds(ui->categories->resultList());

  QString title = ui->titleLineEdit->text();
  QString publicationDate = ui->dateEdit->date().toString(Qt::ISODate);
  QString coverPath = ui->coverLabel->imagePath();

  Book book(title, publicationDate, coverPath, 0);

  LibraryDatabase::insert(book)
    .then(this,
          [author_ids = std::move(author_ids)](QVariant id) {
            BookAuthor::update(id.toUInt(), author_ids).waitForFinished();

            return id.toUInt();
          })
    .then(this,
          [category_ids, this](quint32 book_id) {
            Category::update(book_id, category_ids).waitForFinished();

            emit inserted(book_id);
          })
    .onFailed(this,
              [this](const QSqlError &e) { databaseErrorMessageBox(this, e); });

  QDialog::accept();
}
