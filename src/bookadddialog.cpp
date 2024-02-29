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

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &BookAddDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &BookAddDialog::reject);
}

BookAddDialog::~BookAddDialog() {
  delete ui;
}

static QList<quint32> getIds(QList<QStandardItem *> items) {
  QList<quint32> ids;
  for (QStandardItem *item : items) {
    ids << item->data().toUInt();
  }

  return ids;
}

void BookAddDialog::accept() {
  if (ui->titleLineEdit->text().isEmpty()) {
    return;
  }

  QList<quint32> author_ids = getIds(ui->authors->resultList());
  QList<quint32> category_ids = getIds(ui->categories->resultList());

  QString title = ui->titleLineEdit->text();
  QDate publicationDate = ui->dateEdit->date();
  QString coverPath = ui->coverLabel->imagePath();
  QString description = ui->descriptionText->toPlainText();

  Book book(title, description, publicationDate, coverPath, 0);

  LibraryDatabase::insert(book)
    .then(QtFuture::Launch::Async,
          [author_ids = std::move(author_ids)](QVariant id) {
            if (!author_ids.isEmpty()) {
              BookAuthor::update(id.toUInt(), author_ids).waitForFinished();
            }

            return id.toUInt();
          })
    .then(QtFuture::Launch::Async,
          [category_ids = std::move(category_ids), this](quint32 book_id) {
            if (!category_ids.isEmpty()) {
              BookCategory::update(book_id, category_ids).waitForFinished();
            }
            emit inserted(book_id);
          })
    .onFailed(this, [this](const QSqlError &err) {
      databaseErrorMessageBox(this, err);
    });

  QDialog::accept();
}

void BookAddDialog::open() {
  if (!ui->categories->hasResults()) {
    updateCategories();
  }
  if (!ui->authors->hasResults()) {
    updateAuthors();
  }

  QDialog::open();
};

void BookAddDialog::updateAuthors() {
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
}

void BookAddDialog::updateCategories() {
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
}
