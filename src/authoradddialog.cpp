#include "authoradddialog.h"
#include "ui_authoradddialog.h"

#include "network/networkerror.h"

#include "common/error.h"
#include "common/widgetutils.h"

#include "controllers/authorcontroller.h"

AuthorAddDialog::AuthorAddDialog(QWidget *parent)
    : QDialog(parent), ui(new Ui::AuthorAddDialog) {
  ui->setupUi(this);

  connect(ui->buttonBox, &QDialogButtonBox::accepted, this,
          &AuthorAddDialog::accept);
  connect(ui->buttonBox, &QDialogButtonBox::rejected, this,
          &AuthorAddDialog::reject);
}

void AuthorAddDialog::createAuthor() {
  init();

  m_isUpdating = false;

  open();
}

AuthorAddDialog::~AuthorAddDialog() {
  delete ui;
}

void AuthorAddDialog::init() {
  ui->firstName->clear();
  ui->lastName->clear();
  ui->bio->clear();
  ui->authorImage->setPixmap(QPixmap(":/images/DefaultBookCover"));
}

void AuthorAddDialog::editAuthor(quint32 id) {
  AuthorController controller;

  controller.get(id).then(this, [this](const AuthorDetails &details) {
    m_isUpdating = true;
    m_id = details.id;

    ui->firstName->setText(details.firstName);
    ui->lastName->setText(details.lastName);
    /* TODO: implement */
    // ui->bio->setText(details.bio);
    WidgetUtils::asyncLoadImage(ui->authorImage, details.imageUrl);
    open();
  });
}

void AuthorAddDialog::accept() {
  AuthorController controller;

  Author author;
  author.imageUrl = QUrl::fromLocalFile(ui->authorImage->imagePath());
  author.firstName = ui->firstName->text();
  author.lastName = ui->lastName->text();

  if (m_isUpdating) {
    controller.update(m_id, author).then(this, [this]() {
      emit edited(m_id);
    }).onFailed(this, [this](const NetworkError &err) {
      handleError(this, err);
    });
  } else {
    controller.create(author)
      .then(this, [this](quint32 id) {
      emit edited(id);
    }).onFailed(this, [this](const NetworkError &err) {
      handleError(this, err);
    });
  }

  QDialog::accept();
}
