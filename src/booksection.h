#ifndef BOOKSECTION_H
#define BOOKSECTION_H

#include <QFuture>
#include <QWidget>

#include "network/network.h"
#include "schema/schema.h"

class QStandardItemModel;

class BookSectionDao;
class BookCard;
class BookCardData;
class BookAddDialog;
class SearchFilterDialog;

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
  void synchronizeNowButtonClicked();
  void searchTextChanged(const QString &text);
  void addButtonClicked();
  void updateButtonClicked();
  void bookInsertedHandle();
  void saveChanges();
  void deleteButtonClicked();

  void onBookData(const QList<BookData> &bookCards);

private:
  void loadPage(int pageNumber);

  void reloadCurrentPage();
  QFuture<void> updateNumberOfBooks();

  void distributeGridSize();
  void updateLastSync();
  void hideItems(int itemStart = 0);

  void setBooksCount(int booksCount);
  void processBookUrl(BookCard *bookCard, const QUrl &url);

private:
  Ui::BookSection *ui;

  BookSectionDao *m_dao;
  BookAddDialog *m_bookAddDialog;
  SearchFilterDialog *m_searchFilterDialog;

  QList<ReplyPointer> m_imageReplies;

  int m_booksCount;
};

#endif  // BOOKSECTION_H
