#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QWheelEvent>

#include "smoothscrollarea.h"
#include "smoothscrollbar.h"

SmoothScrollArea::SmoothScrollArea(QWidget *parent) : QScrollArea(parent) {
  m_hScrollBar = new SmoothScrollBar(this);
  m_vScrollBar = new SmoothScrollBar(this);

  setHorizontalScrollBar(m_hScrollBar);
  setVerticalScrollBar(m_vScrollBar);
}

void SmoothScrollArea::setScrollAnimation(Qt::Orientation orientation,
                                          int durationms, QEasingCurve curve) {
  SmoothScrollBar *bar =
    orientation == Qt::Horizontal ? m_hScrollBar : m_vScrollBar;

  bar->setAnimationDuration(durationms);
  bar->setAnimationCurve(curve);
}
