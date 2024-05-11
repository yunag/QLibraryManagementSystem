#include "aspectratiolabel.h"

AspectRatioLabel::AspectRatioLabel(QWidget *parent, Qt::AspectRatioMode mode)
    : QLabel(parent), m_mode(mode) {
  setMinimumSize(1, 1);
  setScaledContents(false);
}

void AspectRatioLabel::resizeEvent(QResizeEvent *) {
  if (!m_pixmap.isNull()) {
    QLabel::setPixmap(scaledPixmap());
  }
}

void AspectRatioLabel::setPixmap(const QPixmap &pixmap) {
  m_pixmap = pixmap;

  QLabel::setPixmap(scaledPixmap());
}

QPixmap AspectRatioLabel::scaledPixmap() const {
  return m_pixmap.scaled(size(), m_mode, Qt::SmoothTransformation);
}

void AspectRatioLabel::setAspectRatio(Qt::AspectRatioMode mode) {
  m_mode = mode;

  if (!pixmap().isNull()) {
    QLabel::setPixmap(scaledPixmap());
  }
}

Qt::AspectRatioMode AspectRatioLabel::aspectRatio() {
  return m_mode;
}

QSize AspectRatioLabel::sizeHint() const {
  return {width(), heightForWidth(width())};
};

int AspectRatioLabel::heightForWidth(int width) const {
  return m_pixmap.isNull() ? height()
                           : m_pixmap.height() * width / m_pixmap.width();
}
