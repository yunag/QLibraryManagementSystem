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
  QSize coverSize();

  void setTitle(const QString &title);
  QString title();

  void setBookId(quint32 id);
  quint32 bookId();

  void setAuthors(const QStringList &authors);
  QString author();

  void setCategories(const QStringList &categories);
  QString category();

  void setRating(int rating);
  int rating();

signals:
  void edited(Book book);

private slots:
  void copyButtonClicked();

private:
  Ui::BookCard *ui;
};

#endif  // BOOKCARD_H
