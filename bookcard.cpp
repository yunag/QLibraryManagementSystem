#include <QClipboard>

#include "bookcard.h"
#include "ui_bookcard.h"

BookCard::BookCard(QWidget *parent) : QWidget(parent), ui(new Ui::BookCard) {
  ui->setupUi(this);

  QPixmap starPixmap(":/resources/images/starRating");
  ui->ratingWidget->setAlignment(Qt::AlignLeft);
  ui->ratingWidget->setCustomPixmap(starPixmap);

  connect(ui->copyButton, &QPushButton::clicked, this,
          &BookCard::copyButtonClicked);
}

BookCard::~BookCard() {
  delete ui;
}

void BookCard::setCover(const QPixmap &pixmap) {
  int w = ui->bookCoverLabel->width();
  int h = ui->bookCoverLabel->height();

  ui->bookCoverLabel->setPixmap(pixmap.scaled(w, h));
}

QSize BookCard::coverSize() const {
  return ui->bookCoverLabel->size();
}

void BookCard::setTitle(const QString &title) {
  ui->titleLineEdit->setText(title);
}

QString BookCard::title() const {
  return ui->titleLineEdit->text();
}

void BookCard::setBookId(quint32 id) {
  ui->bookIDLineEdit->setText(QString::number(id));
}

quint32 BookCard::bookId() const {
  return ui->bookIDLineEdit->text().toUInt();
}

void BookCard::setAuthors(const QStringList &authors) {
  QString labelName = authors.count() > 1 ? tr("Authors") : tr("Author");
  ui->authorLabel->setText(labelName);

  ui->authorLineEdit->setText(authors.join(", "));
}

QString BookCard::author() const {
  return ui->authorLineEdit->text();
}

void BookCard::setCategories(const QStringList &categories) {
  QString labelName =
    categories.count() > 1 ? tr("Categories") : tr("Category");
  ui->categoryLabel->setText(labelName);

  ui->categoryLineEdit->setText(categories.join(", "));
}

QString BookCard::category() const {
  return ui->categoryLineEdit->text();
}

void BookCard::setRating(int rating) {
  ui->ratingWidget->setRating(rating);
}

int BookCard::rating() const {
  return ui->ratingWidget->rating();
}

void BookCard::copyButtonClicked() {
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(ui->titleLineEdit->text());
}
