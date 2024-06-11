#ifndef BOOKADDDIALOG_H
#define BOOKADDDIALOG_H

#include <QDialog>
#include <QFuture>

#include "controllers/bookcontroller.h"
#include "schema/schema.h"

namespace Ui {
class BookAddDialog;
}

class ErrorMessagePopup;

class BookDialogStrategy {
public:
  virtual ~BookDialogStrategy() = default;

  virtual void onOpen() {}

  virtual void onAccept(const Book &) {}
};

class BookAddDialog;

class BookUpdateStrategy : public BookDialogStrategy {
public:
  BookUpdateStrategy(BookAddDialog *dialog) : m_dialog(dialog) {}

  void setBookDetails(const BookDetails &details) { m_bookDetails = details; }

  void onOpen() override;
  void onAccept(const Book &book) override;

private:
  BookDetails m_bookDetails;

  BookAddDialog *m_dialog;
};

class BookCreateStrategy : public BookDialogStrategy {
public:
  BookCreateStrategy(BookAddDialog *dialog) : m_dialog(dialog) {}

  void onAccept(const Book &book) override;

private:
  BookAddDialog *m_dialog;
};

class BookAddDialog : public QDialog {
  Q_OBJECT

  friend BookCreateStrategy;
  friend BookUpdateStrategy;

public:
  explicit BookAddDialog(QWidget *parent = nullptr);
  ~BookAddDialog() override;

  void setStrategy(BookDialogStrategy *strategy) { m_strategy = strategy; }

  void init();
  void editBook(quint32 bookId);
  void createBook();

  void open() override;
  void accept() override;
  QFuture<void> fetchCategories();

signals:
  void edited(quint32 bookId);

private slots:
  void authorsPickerFinished(const QList<Author> &authors);
  void authorsSelectButtonClicked();

private:
  BookDialogStrategy *m_strategy;

  ErrorMessagePopup *m_errorMessagePopup;
  BookUpdateStrategy *m_updateStrategy;
  BookCreateStrategy *m_createStrategy;

  Ui::BookAddDialog *ui;
};

#endif  // BOOKADDDIALOG_H
