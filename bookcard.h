#ifndef BOOKCARD_H
#define BOOKCARD_H

#include <QWidget>

#include "book.h"

namespace Ui {
class BookCard;
}

struct BookCardData {
  BookCardData() = default;
  BookCardData(QPixmap cover_, QString title_, quint32 bookId_,
               QStringList authors_, QStringList categories_, int rating_)
      : cover(std::move(cover_)), title(std::move(title_)), bookId(bookId_),
        authors(std::move(authors_)), categories(std::move(categories_)),
        rating(rating_){};

  QPixmap cover;
  QString title;
  quint32 bookId;

  QStringList authors;
  QStringList categories;

  int rating;
};

class BookCard : public QWidget {
  Q_OBJECT

public:
  explicit BookCard(QWidget *parent = nullptr);
  ~BookCard();

  void setCover(const QPixmap &pixmap);
  QSize coverSize() const;

  void setTitle(const QString &title);
  QString title() const;

  void setBookId(quint32 id);
  quint32 bookId() const;

  void setAuthors(const QStringList &authors);
  QString author() const;

  void setCategories(const QStringList &categories);
  QString category() const;

  void setRating(int rating);
  int rating() const;

signals:
  void edited(Book book);

private slots:
  void copyButtonClicked();

private:
  Ui::BookCard *ui;
};

#endif  // BOOKCARD_H
