#ifndef BOOKSECTION_H
#define BOOKSECTION_H

#include <QFuture>
#include <QWidget>

#include "schema/schema.h"

class QStandardItemModel;

class BookCard;
class BookCardData;
class BookAddDialog;
class SearchFilterDialog;
class BookRestModel;

namespace Ui {
class BookSection;
}

class BookSection : public QWidget {
  Q_OBJECT

public:
  explicit BookSection(QWidget *parent = nullptr);
  ~BookSection() override;

  void loadBooks();

signals:
  void bookDetailsRequested(quint32 bookId);

protected:
  void resizeEvent(QResizeEvent *event) override;

private slots:
  void loadPage();

  void synchronizeNowButtonClicked();
  void searchTextChanged(const QString &text);
  void addButtonClicked();
  void updateButtonClicked();
  void saveChanges();
  void deleteButtonClicked();

private:
  void distributeGridSize();
  void updateLastSync();

  void setBooksCount(int booksCount);
  void processBookUrl(BookCard *bookCard, const QUrl &url);

private:
  Ui::BookSection *ui;

  BookRestModel *m_model;
  BookAddDialog *m_bookAddDialog;
  SearchFilterDialog *m_searchFilterDialog;
};

#endif  // BOOKSECTION_H
