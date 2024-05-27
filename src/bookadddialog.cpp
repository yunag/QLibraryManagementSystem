#include <QFuture>
#include <QFutureSynchronizer>
#include <QMessageBox>
#include <QStandardItem>
#include <QtConcurrent>

#include <QJsonArray>
#include <QJsonObject>

#include "bookadddialog.h"

#include "controllers/authorbookcontroller.h"
#include "controllers/authorcontroller.h"
#include "controllers/bookcategorycontroller.h"
#include "controllers/bookcontroller.h"
#include "controllers/categorycontroller.h"

#include "common/error.h"
#include "common/future.h"
#include "common/json.h"
#include "common/widgetutils.h"

#include "ui_bookadddialog.h"

BookAddDialog::BookAddDialog(QWidget *parent)
    : QDialog(parent), m_updateStrategy(new BookUpdateStrategy(this)),
      m_createStrategy(new BookCreateStrategy(this)),
      ui(new Ui::BookAddDialog) {
  ui->setupUi(this);

  ui->coverLabel->setAspectRatio(Qt::KeepAspectRatio);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &BookAddDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &BookAddDialog::reject);
}

BookAddDialog::~BookAddDialog() {
  delete ui;
}

template <typename T>
QList<quint32> grabIds(const QList<T> &list) {
  QList<quint32> ids;
  for (const auto &item : list) {
    ids << item.id;
  }
  return ids;
}

QList<quint32> grabIds(const QList<QStandardItem *> &items) {
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

  QList<quint32> categoryIds = grabIds(ui->categories->rightList());

  Book book;
  book.title = ui->titleLineEdit->text();
  book.description = ui->descriptionText->toPlainText();
  book.publicationDate = ui->publicationDate->date();
  book.coverUrl = QUrl::fromLocalFile(ui->coverLabel->imagePath());
  book.copiesOwned = ui->copiesOwned->text().toInt();

  m_strategy->onAccept(book);

  QDialog::accept();
}

void BookAddDialog::init() {
  for (auto *const item : ui->authors->rightList()) {
    ui->authors->swapRightToLeft(item->index());
  }

  for (auto *const item : ui->categories->rightList()) {
    ui->categories->swapRightToLeft(item->index());
  }

  ui->coverLabel->setPixmap(QPixmap(":/images/DefaultBookCover"));

  ui->titleLineEdit->setText("");
  ui->copiesOwned->setValue(0);
  ui->publicationDate->setDate(QDate());
  ui->descriptionText->setPlainText("");
}

void BookAddDialog::open() {
  auto syncronizer = makeFutureSyncronizer<void>();

  if (ui->categories->leftList().isEmpty()) {
    syncronizer->addFuture(fetchCategories());
  }
  if (ui->authors->leftList().isEmpty()) {
    syncronizer->addFuture(fetchAuthors());
  }

  QtConcurrent::run([syncronizer] {})
    .then(QtFuture::Launch::Async,
          [syncronizer] { syncronizer->waitForFinished(); })
    .then(this, [this]() {
    init();
    m_strategy->onOpen();
  }).onFailed(this, [this](const NetworkError &err) {
    handleError(this, err);
  });

  QDialog::open();
};

QFuture<void> BookAddDialog::fetchAuthors() {
  return AuthorController::getAuthors().then(
    this, [this](const QList<Author> &authors) {
    QList<QStandardItem *> authorsItem;
    for (const auto &author : authors) {
      auto *authorItem =
        new QStandardItem(author.firstName + " " + author.lastName);

      authorItem->setData(author.id);
      authorsItem.push_back(authorItem);
    }

    ui->authors->addItems(authorsItem);
  });
}

QFuture<void> BookAddDialog::fetchCategories() {
  return CategoryController::getCategories().then(
    [this](const QList<Category> &categories) {
    QList<QStandardItem *> categoriesItems;

    for (const auto &category : categories) {
      auto *categoryItem = new QStandardItem(category.name);

      categoryItem->setData(category.id);
      categoriesItems.push_back(categoryItem);
    }

    ui->categories->addItems(categoriesItems);
  });
}

void BookUpdateStrategy::onOpen() {
  Ui::BookAddDialog *ui = m_dialog->ui;

  auto authorsId = grabIds(m_bookData.authors);
  for (auto *const item : ui->authors->leftList()) {
    quint32 authorId = item->data().toUInt();

    if (authorsId.contains(authorId)) {
      ui->authors->swapLeftToRight(item->index());
    }
  }

  auto categoriesId = grabIds(m_bookData.categories);
  for (auto *const item : ui->categories->leftList()) {
    quint32 categoryId = item->data().toUInt();

    if (categoriesId.contains(categoryId)) {
      ui->categories->swapLeftToRight(item->index());
    }
  }

  WidgetUtils::asyncLoadImage(ui->coverLabel, m_bookData.coverUrl);

  ui->titleLineEdit->setText(m_bookData.title);
  ui->copiesOwned->setValue(m_bookData.copiesOwned);
  ui->publicationDate->setDate(m_bookData.publicationDate);
  ui->descriptionText->setPlainText(m_bookData.description);
}

void BookUpdateStrategy::onAccept(const Book &bookWithoutId) {
  Ui::BookAddDialog *ui = m_dialog->ui;

  auto syncronizer = makeFutureSyncronizer<QByteArray>();

  Book bookWithId = bookWithoutId;
  bookWithId.id = m_bookData.id;

  BookController::updateBook(bookWithId)
    .then(m_dialog,
          [syncronizer, this, ui](auto) {
    syncronizer->addFuture(AuthorBookController::updateRelations(
      m_bookData.id, grabIds(ui->authors->rightList())));

    syncronizer->addFuture(BookCategoryController::updateRelations(
      m_bookData.id, grabIds(ui->categories->rightList())));
  })
    .then(QtFuture::Launch::Async, [syncronizer, this]() {
    syncronizer->waitForFinished();
    emit m_dialog->edited(m_bookData.id);
  }).onFailed(m_dialog, [this](const NetworkError &err) {
    handleError(m_dialog, err);
  });
}

void BookCreateStrategy::onAccept(const Book &book) {
  Ui::BookAddDialog *ui = m_dialog->ui;

  auto syncronizer = makeFutureSyncronizer<QByteArray>();

  BookController::createBook(book)
    .then(m_dialog,
          [syncronizer, ui](quint32 bookId) {
    syncronizer->addFuture(AuthorBookController::updateRelations(
      bookId, grabIds(ui->authors->rightList())));

    syncronizer->addFuture(BookCategoryController::updateRelations(
      bookId, grabIds(ui->categories->rightList())));

    return bookId;
  })
    .then(QtFuture::Launch::Async, [syncronizer, this](quint32 bookId) {
    syncronizer->waitForFinished();
    emit m_dialog->edited(bookId);
  }).onFailed(m_dialog, [this](const NetworkError &err) {
    handleError(m_dialog, err);
  });
}

void BookAddDialog::editBook(quint32 bookId) {
  m_strategy = m_updateStrategy;

  BookController::getBookById(bookId).then([this](const BookData &data) {
    m_updateStrategy->setBookData(data);
    open();
  });
}

void BookAddDialog::createBook() {
  m_strategy = m_createStrategy;

  open();
}
