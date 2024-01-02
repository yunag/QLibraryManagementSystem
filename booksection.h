#ifndef BOOKSECTION_H
#define BOOKSECTION_H

#include <QListWidgetItem>
#include <QSqlQuery>
#include <QWidget>

#include "bookcard.h"
#include "qlibrarydatabase.h"

namespace Ui {
class BookSection;
}

class BookSection : public QWidget {
  Q_OBJECT

public:
  static constexpr int itemsPerPage = 20;

public:
  explicit BookSection(QLibraryDatabase &db, QWidget *parent = nullptr);
  ~BookSection();

  void loadBooks();

protected:
  virtual void resizeEvent(QResizeEvent *event) override;

private:
  enum Condition { Search = 0, FilterCategory, _Last = FilterCategory };

private:
  void setCondition(Condition cond, const QString &condStr = "");
  QString applyConditions();

  bool loadPage(qint32 pageNumber);
  void distributeGridSize();
  void updateLastSync();
  void updateNumberOfBooks();
  void hideItems(quint32 itemStart = 0);

  bool isEndPage(qint32 pageNumber);
  bool isStartPage(qint32 pageNumber);

signals:
  void itemProcessed(quint32 itemNumber, const QPixmap &cover, quint32 book_id,
                     const QString &title, const QString &category,
                     const QString &author);
  void pageDone(quint32 itemNumber);

public slots:
  void synchronizeNowButtonClicked();
  void searchTextReturnPressed();
  void nextPageButtonClicked();
  void prevPageButtonClicked();

  void pageDoneHandler(quint32 itemNumber);
  void itemProcessedHandler(quint32 itemNumber, const QPixmap &cover,
                            quint32 book_id, const QString &title,
                            const QString &category, const QString &author);

private:
  Ui::BookSection *ui;
  QLibraryDatabase &m_database;

  BookCard *m_bookCards[itemsPerPage];
  QListWidgetItem *m_bookItems[itemsPerPage];

  QString m_conditions[_Last + 1];

  bool m_pageLoading;

  qint32 m_currentPage;
  quint32 m_booksCount;
};

#endif // BOOKSECTION_H
