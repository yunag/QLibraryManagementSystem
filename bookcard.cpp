#include "bookcard.h"
#include "ui_bookcard.h"

BookCard::BookCard(QWidget *parent) : QWidget(parent), ui(new Ui::BookCard) {
  ui->setupUi(this);
}

BookCard::~BookCard() { delete ui; }

void lineEditSetTextMoveCursor(QLineEdit *lineEdit, const QString &text,
                               int cursor = 0) {
  lineEdit->setText(text);
  lineEdit->setCursorPosition(cursor);
}

void BookCard::setCover(const QPixmap &pixmap) {
  int w = ui->bookCoverLabel->width();
  int h = ui->bookCoverLabel->height();

  ui->bookCoverLabel->setPixmap(pixmap.scaled(w, h));
}

QSize BookCard::coverSize() { return ui->bookCoverLabel->size(); }

void BookCard::setTitle(const QString &title) {
  lineEditSetTextMoveCursor(ui->titleLineEdit, title);
}

QString BookCard::title() { return ui->titleLineEdit->text(); }

void BookCard::setBookId(quint32 id) {
  lineEditSetTextMoveCursor(ui->bookIDLineEdit, QString::number(id));
}

quint32 BookCard::bookId() { return ui->bookIDLineEdit->text().toUInt(); }

void BookCard::setAuthor(const QString &author) {
  QString labelName = author.contains(',') ? "Authors" : "Author";
  ui->authorLabel->setText(labelName);

  lineEditSetTextMoveCursor(ui->authorLineEdit, author);
}

QString BookCard::author() { return ui->authorLineEdit->text(); }

void BookCard::setCategory(const QString &category) {
  QString labelName = category.contains(',') ? "Categories" : "Category";
  ui->categoryLabel->setText(labelName);

  lineEditSetTextMoveCursor(ui->categoryLineEdit, category);
}

QString BookCard::category() { return ui->categoryLineEdit->text(); }

void BookCard::setRating(int rating) {
  lineEditSetTextMoveCursor(ui->ratingLineEdit, QString::number(rating));
}
int BookCard::rating() { return ui->ratingLineEdit->text().toInt(); }
