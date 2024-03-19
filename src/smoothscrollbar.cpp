#include <QPropertyAnimation>
#include <QWheelEvent>

#include "smoothscrollbar.h"

SmoothScrollBar::SmoothScrollBar(QWidget *parent)
    : SmoothScrollBar(QEasingCurve::OutCubic, 500, parent) {}

SmoothScrollBar::SmoothScrollBar(QEasingCurve curve, int duration,
                                 QWidget *parent)
    : QScrollBar(parent) {
  m_animation = new QPropertyAnimation(this, "value", this);
  m_animation->setEasingCurve(curve);
  m_animation->setDuration(duration);
  m_animation->setEndValue(value());

  connect(m_animation, &QPropertyAnimation::finished, this,
          &SmoothScrollBar::scrollFinished);
}

void SmoothScrollBar::scrollValue(int newValue) {
  if (newValue == value())
    return;

  m_animation->stop();
  emit scrollFinished();

  m_animation->setStartValue(value());
  m_animation->setEndValue(newValue);
  m_animation->start();
}

void SmoothScrollBar::mousePressEvent(QMouseEvent *event) {
  m_animation->stop();
  QScrollBar::mousePressEvent(event);
  m_animation->setEndValue(value());
}

void SmoothScrollBar::mouseReleaseEvent(QMouseEvent *event) {
  m_animation->stop();
  QScrollBar::mouseReleaseEvent(event);
  m_animation->setEndValue(value());
}

void SmoothScrollBar::mouseMoveEvent(QMouseEvent *event) {
  m_animation->stop();
  QScrollBar::mouseMoveEvent(event);
  m_animation->setEndValue(value());
}

void SmoothScrollBar::wheelEvent(QWheelEvent *event) {
  int minVal = minimum(), maxVal = maximum();
  int curVal = value();

  int decrement = -event->angleDelta().x() - event->angleDelta().y();
  int endVal = qBound(minVal, m_animation->endValue().toInt(), maxVal);

  bool isUpBlock = curVal == maxVal && curVal + decrement >= maxVal;
  bool isDownBlock = curVal == minVal && curVal + decrement <= minVal;

  if (isUpBlock || isDownBlock) {
    event->ignore();
  } else {
    scrollValue(endVal + decrement);
    event->accept();
  }
}

void SmoothScrollBar::setAnimationDuration(int msecs) {
  m_animation->setDuration(msecs);
}

void SmoothScrollBar::setAnimationCurve(QEasingCurve curve) {
  m_animation->setEasingCurve(curve);
}
