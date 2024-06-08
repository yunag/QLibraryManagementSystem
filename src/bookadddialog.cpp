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

#include "common/algorithm.h"
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

  connect(ui->hideButton, &QPushButton::clicked, this,
          [this](auto) { hide(); });

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
  return algorithm::transform<QList<quint32>>(
    list, [](const T &item) { return item.id; });
}

QList<quint32> grabIds(const QList<QStandardItem *> &items) {
  return algorithm::transform<QList<quint32>>(
    items, [](const QStandardItem *item) { return item->data().toUInt(); });
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
  AuthorController controller;
  return controller.getAuthors().then(this,
                                      [this](const QList<Author> &authors) {
    auto items = algorithm::transform<QList<QStandardItem *>>(
      authors, [](const Author &author) {
      auto *item = new QStandardItem(author.firstName + " " + author.lastName);
      item->setData(author.id);
      return item;
    });

    ui->authors->addItems(items);
  });
}

QFuture<void> BookAddDialog::fetchCategories() {
  return CategoryController::getCategories().then(
    [this](const QList<Category> &categories) {
    auto items = algorithm::transform<QList<QStandardItem *>>(
      categories, [](const Category &category) {
      auto *item = new QStandardItem(category.name);
      item->setData(category.id);
      return item;
    });

    ui->categories->addItems(items);
  });
}

void BookUpdateStrategy::onOpen() {
  Ui::BookAddDialog *ui = m_dialog->ui;

  auto authorsId = grabIds(m_bookDetails.authors);

  for (auto *const item : ui->authors->leftList()) {
    quint32 authorId = item->data().toUInt();

    if (authorsId.contains(authorId)) {
      ui->authors->swapLeftToRight(item->index());
    }
  }

  auto categoriesId = grabIds(m_bookDetails.categories);
  for (auto *const item : ui->categories->leftList()) {
    quint32 categoryId = item->data().toUInt();

    if (categoriesId.contains(categoryId)) {
      ui->categories->swapLeftToRight(item->index());
    }
  }

  WidgetUtils::asyncLoadImage(ui->coverLabel, m_bookDetails.coverUrl);

  ui->titleLineEdit->setText(m_bookDetails.title);
  ui->copiesOwned->setValue(m_bookDetails.copiesOwned);
  ui->publicationDate->setDate(m_bookDetails.publicationDate);
  ui->descriptionText->setPlainText(m_bookDetails.description);
}

void BookUpdateStrategy::onAccept(const Book &book) {
  Ui::BookAddDialog *ui = m_dialog->ui;

  auto syncronizer = makeFutureSyncronizer<QByteArray>();

  BookController controller;
  controller.update(m_bookDetails.id, book)
    .then(m_dialog,
          [syncronizer, this, ui]() {
    syncronizer->addFuture(AuthorBookController::updateRelations(
      m_bookDetails.id, grabIds(ui->authors->rightList())));

    syncronizer->addFuture(BookCategoryController::updateRelations(
      m_bookDetails.id, grabIds(ui->categories->rightList())));
  })
    .then(QtFuture::Launch::Async, [syncronizer, this]() {
    syncronizer->waitForFinished();
    emit m_dialog->edited(m_bookDetails.id);
  }).onFailed(m_dialog, [this](const NetworkError &err) {
    handleError(m_dialog, err);
  });
}

void BookCreateStrategy::onAccept(const Book &book) {
  Ui::BookAddDialog *ui = m_dialog->ui;

  auto syncronizer = makeFutureSyncronizer<QByteArray>();

  BookController controller;
  controller.create(book)
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

  BookController controller;
  controller.get(bookId).then([this](const BookDetails &data) {
    m_updateStrategy->setBookDetails(data);
    open();
  });
}

void BookAddDialog::createBook() {
  m_strategy = m_createStrategy;

  open();
}
