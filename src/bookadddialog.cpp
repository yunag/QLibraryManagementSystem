#include <QStandardItemModel>
#include <QtConcurrent>

#include <QJsonArray>
#include <QJsonObject>
#include <QRegularExpressionValidator>

#include <QToolTip>

#include "bookadddialog.h"

#include "errormessagepopup.h"

#include "controllers/authorbookcontroller.h"
#include "controllers/authorcontroller.h"
#include "controllers/bookcategorycontroller.h"
#include "controllers/bookcontroller.h"
#include "controllers/categorycontroller.h"

#include "model/authorrestmodel.h"

#include "common/algorithm.h"
#include "common/error.h"
#include "common/future.h"
#include "common/json.h"
#include "common/widgetutils.h"

#include "libraryapplication.h"
#include "librarymainwindow.h"

#include "ui_bookadddialog.h"

BookAddDialog::BookAddDialog(QWidget *parent)
    : QDialog(parent), m_errorMessagePopup(new ErrorMessagePopup(this)),
      m_updateStrategy(new BookUpdateStrategy(this)),
      m_createStrategy(new BookCreateStrategy(this)),
      ui(new Ui::BookAddDialog) {
  ui->setupUi(this);

  ui->coverLabel->setAspectRatio(Qt::KeepAspectRatio);
  ui->titleLineEdit->setValidator(new QRegularExpressionValidator(
    QRegularExpression(R"(^[^\s]+(\s+[^\s]+)*$)"), this));

  connect(App->mainWindow(), &LibraryMainWindow::authorsPickerFinished, this,
          &BookAddDialog::authorsPickerFinished);

  connect(ui->authorsSelectButton, &QPushButton::clicked, this,
          &BookAddDialog::authorsSelectButtonClicked);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &BookAddDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &BookAddDialog::reject);
  connect(ui->categories, &TwoWayList::syncButtonClicked, this,
          &BookAddDialog::fetchCategories);
}

BookAddDialog::~BookAddDialog() {
  delete ui;
}

template <typename T>
QList<quint32> grabIds(const QList<T> &list) {
  return algorithm::transform<QList<quint32>>(
    list, [](const T &item) { return item.id; });
}

QList<quint32> grabIds(QStandardItemModel *model) {
  return algorithm::transform<QList<quint32>>(
    model->findItems("*", Qt::MatchWildcard),
    [](QStandardItem *item) { return item->data().toUInt(); });
}

QList<quint32> grabIds(QListWidget *listWidget) {
  return algorithm::transform<QList<quint32>>(
    listWidget->findItems("*", Qt::MatchWildcard), [](QListWidgetItem *item) {
    return item->data(AuthorRestModel::IdRole).toUInt();
  });
}

void BookAddDialog::accept() {
  if (!ui->titleLineEdit->hasAcceptableInput()) {
    m_errorMessagePopup->showMessage(ui->titleLineEdit,
                                     tr("Title must be non-empty string"));
    return;
  }

  QList<quint32> categoryIds = grabIds(ui->categories->rightModel());

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
  for (QStandardItem *item :
       ui->categories->rightModel()->findItems("*", Qt::MatchWildcard)) {
    ui->categories->swapRightToLeft(item->index());
  }

  ui->coverLabel->setPixmap(QPixmap(":/images/DefaultBookCover"));

  ui->authors->clear();
  ui->titleLineEdit->setText("");
  ui->copiesOwned->setValue(0);
  ui->publicationDate->setDate(QDate());
  ui->descriptionText->setPlainText("");
}

void BookAddDialog::open() {
  QDialog::open();

  init();

  if (ui->categories->leftModel()->rowCount()) {
    m_strategy->onOpen();
    return;
  }

  fetchCategories().then(this, [this]() { m_strategy->onOpen(); });
};

QFuture<void> BookAddDialog::fetchCategories() {
  ui->categories->leftModel()->clear();
  ui->categories->rightModel()->clear();

  CategoryController controller;

  return controller.getCategories()
    .then([this](const QList<Category> &categories) {
    auto items = algorithm::transform<QList<QStandardItem *>>(
      categories, [](const Category &category) {
      auto *item = new QStandardItem(category.name);
      item->setData(category.id);
      return item;
    });

    ui->categories->addItems(items);
  }).onFailed(this, [this](const NetworkError &err) {
    handleError(this, err);
  });
}

void BookUpdateStrategy::onOpen() {
  Ui::BookAddDialog *ui = m_dialog->ui;

  auto categoriesId = grabIds(m_bookDetails.categories);
  for (QStandardItem *item :
       ui->categories->leftModel()->findItems("*", Qt::MatchWildcard)) {
    quint32 categoryId = item->data().toUInt();

    if (categoriesId.contains(categoryId)) {
      ui->categories->swapLeftToRight(item->index());
    }
  }

  for (const Author &author : m_bookDetails.authors) {
    QString fullName = author.firstName + " " + author.lastName;
    auto *item = new QListWidgetItem(fullName);
    item->setData(AuthorRestModel::IdRole, author.id);

    ui->authors->addItem(item);
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
      m_bookDetails.id, grabIds(ui->authors)));

    syncronizer->addFuture(BookCategoryController::updateRelations(
      m_bookDetails.id, grabIds(ui->categories->rightModel())));
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
    syncronizer->addFuture(
      AuthorBookController::updateRelations(bookId, grabIds(ui->authors)));

    syncronizer->addFuture(BookCategoryController::updateRelations(
      bookId, grabIds(ui->categories->rightModel())));

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

void BookAddDialog::authorsPickerFinished(const QList<Author> &authors) {
  show();

  for (const auto &author : authors) {
    auto *item = new QListWidgetItem(author.firstName + " " + author.lastName);

    item->setData(AuthorRestModel::IdRole, author.id);

    ui->authors->addItem(item);
  }
}

void BookAddDialog::authorsSelectButtonClicked() {
  ui->authors->clear();

  hide();
  App->mainWindow()->requestAuthorsPicker();
}
