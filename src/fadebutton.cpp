#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include "fadebutton.h"

FadeButton::FadeButton(bool isVisibleInitially, int msecsDuration,
                       const QEasingCurve &curve, QWidget *parent)
    : QPushButton(parent), m_effect(new QGraphicsOpacityEffect(this)),
      m_isFadeIn(isVisibleInitially) {

  m_effect->setOpacity(m_isFadeIn);

  m_animation = new QPropertyAnimation(m_effect, "opacity");
  m_animation->setDuration(msecsDuration);
  m_animation->setEasingCurve(curve);
  m_animation->setStartValue(0.0);
  m_animation->setEndValue(1.0);

  setVisible(m_isFadeIn);

  connect(m_animation, &QPropertyAnimation::finished, this,
          [this]() { setVisible(m_isFadeIn); });

  setGraphicsEffect(m_effect);
}

void FadeButton::fadeIn() {
  if (isFadeIn()) {
    return;
  }

  m_isFadeIn = true;

  setVisible(true);

  fade(QAbstractAnimation::Forward);
}

void FadeButton::fadeOut() {
  if (isFadeOut()) {
    return;
  }

  m_isFadeIn = false;

  fade(QAbstractAnimation::Backward);
}

bool FadeButton::isFadeIn() const {
  return m_isFadeIn;
}

bool FadeButton::isFadeOut() const {
  return !m_isFadeIn;
}

void FadeButton::fade(QAbstractAnimation::Direction direction) {
  m_animation->setDirection(direction);

  if (m_animation->state() != QPropertyAnimation::Running) {
    m_animation->start();
  }
}
