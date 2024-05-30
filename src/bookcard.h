#ifndef BOOKCARD_H
#define BOOKCARD_H

#include <QStyle>
#include <QUrl>
#include <QWidget>

#include <kratingpainter.h>

class QStyleOptionViewItem;

class BookCard {

public:
  explicit BookCard();

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             int hoverRating) const;
  QSize sizeHint() const;

  void setCover(const QPixmap &pixmap);
  QPixmap cover() const;

  void setBusyIndicator(QSharedPointer<QMovie> indicator);

  void setTitle(const QString &title);
  QString title() const;

  void setBookId(quint32 bookId);
  quint32 bookId() const;

  void setAuthors(const QStringList &authors);
  QStringList authors() const;

  void setCategories(const QStringList &categories);
  QStringList categories() const;

  void setRating(int rating);
  int rating() const;

  void setCoverUrl(const QUrl &url);
  QUrl coverUrl() const;

  void setButtonState(QStyle::State buttonState);
  QStyle::State buttonState() const;

  int ratingFromPosition(const QRect &rect, const QPoint &pos);
  QRect buttonRect(const QRect &rect) const;

private:
  KRatingPainter *m_ratingPainter;

  QString m_title;
  QPixmap m_cover;
  QUrl m_coverUrl;
  quint32 m_bookId;

  int m_rating;

  QStringList m_authors;
  QStringList m_categories;

  QStyle::State m_buttonState;

  QWeakPointer<QMovie> m_indicator;
};

Q_DECLARE_METATYPE(BookCard)

#endif  // BOOKCARD_H
