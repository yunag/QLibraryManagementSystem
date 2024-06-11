#include <QApplication>
#include <QMovie>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

#include <kratingpainter.h>

#include "bookcard.h"

BookCard::BookCard() : m_ratingPainter(new KRatingPainter) {
  m_ratingPainter->setAlignment(Qt::AlignLeft);
  m_buttonState = QStyle::State_Enabled;
}

struct CardLayout {
  QRect cover;
  QRect rating;
  QRect title;
  QRect button;
  QRect bookIdL;
  QRect authorsL;
  QRect categoriesL;
  QRect ratingL;
  QRect rightCenter;
};

static const int verticalSpacing = 10;
static const int formRectHeight = 20;
static const int titleBottomMargin = 12;
static const int titleHeight = 30;

static inline CardLayout cardLayout(const QRect &rect) {
  CardLayout l;

  l.cover = rect;

  int w = rect.height() / 1.4f; /* Book Aspect ratio */

  l.cover.setWidth(w);

  const int margin = 20;

  QRect right;
  right.setLeft(l.cover.right() + margin);
  right.setTop(rect.top());
  right.setRight(rect.right() - margin);
  right.setBottom(rect.bottom());

  int requiredRightHeight =
    titleHeight + 4 * formRectHeight + titleBottomMargin + 3 * verticalSpacing;

  l.rightCenter.setWidth(right.width());
  l.rightCenter.setHeight(requiredRightHeight);

  l.rightCenter.moveCenter(right.center());

  l.button.setWidth(titleHeight);
  l.button.setHeight(titleHeight);
  l.button.moveTopRight(l.rightCenter.topRight());

  l.title.setLeft(l.rightCenter.left());
  l.title.setTop(l.rightCenter.top());
  l.title.setRight(l.button.left());
  l.title.setHeight(titleHeight);

  QRect form;
  form.setTop(l.title.bottom() + titleBottomMargin);
  form.setLeft(l.rightCenter.left());
  form.setRight(l.rightCenter.right());
  form.setBottom(l.rightCenter.bottom());

  QRect formRect;
  formRect.setTopLeft(form.topLeft());
  formRect.setWidth(75);
  formRect.setHeight(formRectHeight);

  l.bookIdL = formRect;
  l.authorsL = l.bookIdL.translated(0, formRectHeight + verticalSpacing);
  l.categoriesL = l.authorsL.translated(0, formRectHeight + verticalSpacing);
  l.ratingL = l.categoriesL.translated(0, formRectHeight + verticalSpacing);

  l.rating.setTop(l.ratingL.top());
  l.rating.setLeft(l.ratingL.right() + 10);
  l.rating.setRight(l.rightCenter.right());
  l.rating.setHeight(formRectHeight);

  return l;
}

void BookCard::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     int hoverRating) const {
  QRect rect = option.rect;
  QPalette palette = option.palette;

  painter->save();

  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  if (option.state & QStyle::State_Selected) {
    painter->save();

    int borderWidth = 2;
    QPen borderPen(palette.windowText(), borderWidth);

    painter->setPen(borderPen);
    painter->setBrush(palette.window().color().darker(98));
    painter->drawRect(rect.adjusted(borderWidth / 2, borderWidth / 2,
                                    -borderWidth / 2, -borderWidth / 2));
    painter->restore();
  } else {
    painter->fillRect(rect, palette.window().color().darker(98));
  }

  CardLayout l = cardLayout(rect);

  if (!m_cover.isNull()) {
    QPixmap scaledPixmap = m_cover.scaled(l.cover.size(), Qt::IgnoreAspectRatio,
                                          Qt::SmoothTransformation);

    painter->drawPixmap(l.cover, scaledPixmap);
  } else if (auto indicator = m_indicator.lock()) { /* Show busy indicator */
    int pixmapSize = qMin(l.cover.height() / 2, l.cover.width() / 2);

    QPixmap pixmap = indicator->currentPixmap().scaled(
      pixmapSize, pixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QRect pixmapRect = pixmap.rect();
    pixmapRect.moveCenter(l.cover.center());

    painter->drawPixmap(pixmapRect.topLeft(), pixmap);
  }

  QFont font = painter->font();
  font.setPointSize(14);

  QFontMetrics fm(font);

  /* NOTE: Without this function performance will drop significantly */
  auto textPos = [&](const QRect &rect) {
    return QPoint(rect.x(), rect.y() + fm.ascent());
  };

  painter->setFont(font);
  painter->setBrush(palette.text());
  painter->drawText(textPos(l.title),
                    fm.elidedText(m_title, Qt::ElideRight, l.title.width()));

  QFont labelFont = painter->font();
  labelFont.setPointSize(10);
  labelFont.setBold(true);

  fm = QFontMetrics(labelFont);
  painter->setFont(labelFont);
  painter->drawText(textPos(l.bookIdL), QObject::tr("Book id"));
  painter->drawText(textPos(l.authorsL), QObject::tr("Authors"));
  painter->drawText(textPos(l.categoriesL), QObject::tr("Categories"));
  painter->drawText(textPos(l.ratingL), QObject::tr("Rating"));

  QRect rightForm;
  rightForm.setLeft(l.bookIdL.right() + 10);
  rightForm.setRight(l.rightCenter.right());
  rightForm.setTop(l.bookIdL.top());
  rightForm.setBottom(l.bookIdL.bottom());

  int advance = rightForm.height() + verticalSpacing;

  font.setPointSize(10);
  fm = QFontMetrics(font);

  auto elideText = [&](const QString &text) {
    return fm.elidedText(text, Qt::ElideRight, rightForm.width());
  };

  painter->setFont(font);
  painter->drawText(textPos(rightForm), elideText(QString::number(m_bookId)));
  painter->drawText(textPos(rightForm.translated(0, advance)),
                    elideText(m_authors.join(", ")));
  painter->drawText(textPos(rightForm.translated(0, 2 * advance)),
                    elideText(m_categories.join(", ")));

  QStyleOptionButton buttonOpt;
  buttonOpt.rect = l.button.marginsRemoved(QMargins(1, 1, 1, 1));
  buttonOpt.state = m_buttonState;
  buttonOpt.icon = QIcon::fromTheme("link");
  buttonOpt.iconSize = QSize(24, 24);

  QStyle *style =
    option.widget ? option.widget->style() : QApplication::style();
  style->drawControl(QStyle::CE_PushButton, &buttonOpt, painter);

  m_ratingPainter->paint(painter, l.rating, m_rating, hoverRating);

  painter->restore();
}

QSize BookCard::sizeHint() {
  /* NOTE: hardcoded value */
  return {450, 255};
}

int BookCard::ratingFromPosition(const QRect &rect, const QPoint &pos) const {
  CardLayout layout = cardLayout(rect);
  return m_ratingPainter->ratingFromPosition(layout.rating, pos);
}

void BookCard::setCover(const QPixmap &pixmap) {
  m_cover = pixmap;
}

QPixmap BookCard::cover() const {
  return m_cover;
}

void BookCard::setTitle(const QString &title) {
  m_title = title;
}

QString BookCard::title() const {
  return m_title;
}

void BookCard::setBookId(quint32 bookId) {
  m_bookId = bookId;
}

quint32 BookCard::bookId() const {
  return m_bookId;
}

void BookCard::setAuthors(const QStringList &authors) {
  m_authors = authors;
}

QStringList BookCard::authors() const {
  return m_authors;
}

void BookCard::setCategories(const QStringList &categories) {
  m_categories = categories;
}

QStringList BookCard::categories() const {
  return m_categories;
}

void BookCard::setRating(int rating) {
  m_rating = rating;
}

int BookCard::rating() const {
  return m_rating;
}

void BookCard::setBusyIndicator(QSharedPointer<QMovie> indicator) {
  m_indicator = indicator;
}

QRect BookCard::buttonRect(const QRect &rect) const {
  return cardLayout(rect).button;
}

void BookCard::setButtonState(QStyle::State buttonState) {
  m_buttonState = buttonState;
}

QStyle::State BookCard::buttonState() const {
  return m_buttonState;
}

void BookCard::setCoverUrl(const QUrl &url) {
  m_coverUrl = url;
}

QUrl BookCard::coverUrl() const {
  return m_coverUrl;
}
