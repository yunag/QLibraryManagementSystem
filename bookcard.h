#ifndef BOOKCARD_H
#define BOOKCARD_H

#include <QWidget>

#include "book.h"

namespace Ui {
class BookCard;
}

struct BookCardData {
  BookCardData() = default;

  BookCardData(QPixmap cover, QString title, quint32 bookId,
               QStringList authors, QStringList categories, double rating)
      : cover(std::move(cover)), title(std::move(title)), bookId(bookId),
        authors(std::move(authors)), categories(std::move(categories)),
        rating(rating) {}

  QPixmap cover;
  QString title;
  quint32 bookId;

  QStringList authors;
  QStringList categories;

  double rating;
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
