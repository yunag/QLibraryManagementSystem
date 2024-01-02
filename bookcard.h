#ifndef BOOKCARD_H
#define BOOKCARD_H

#include <QWidget>

namespace Ui {
class BookCard;
}

class BookCard : public QWidget {
  Q_OBJECT

public:
  explicit BookCard(QWidget *parent = nullptr);
  ~BookCard();

  void setCover(const QPixmap &pixmap);

  void setTitle(const QString &title);
  QString title();

  void setBookId(quint32 id);
  quint32 bookId();

  void setAuthor(const QString &author);
  QString author();

  void setCategory(const QString &categroy);
  QString category();

  void setRating(int rating);
  int rating();

private:
  Ui::BookCard *ui;
};

#endif // BOOKCARD_H
