#ifndef BOOKCARD_H
#define BOOKCARD_H

#include <QUrl>
#include <QWidget>

#include "aspectratiolabel.h"

#include "schema/book.h"

namespace Ui {
class BookCard;
}

class BookCard : public QWidget {
  Q_OBJECT

public:
  explicit BookCard(QWidget *parent = nullptr);
  ~BookCard() override;

  AspectRatioLabel *coverLabel();

  void setCover(const QPixmap &pixmap);
  QSize coverSize() const;

  void setTitle(const QString &title);
  QString title() const;

  void setBookId(quint32 bookId);
  quint32 bookId() const;

  void setAuthors(const QStringList &authors);
  QString author() const;

  void setCategories(const QStringList &categories);
  QString category() const;

  void setRating(int rating);
  int rating() const;

signals:
  void edited(Book book);
  void ratingChanged(int rating);

private slots:
  void copyButtonClicked();

private:
  Ui::BookCard *ui;
};

#endif  // BOOKCARD_H
