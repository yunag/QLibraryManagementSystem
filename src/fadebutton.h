#ifndef FADEBUTTON_H
#define FADEBUTTON_H

#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QPushButton>

class QGraphicsOpacityEffect;
class QPropertyAnimation;

class FadeButton : public QPushButton {
  Q_OBJECT

public:
  FadeButton(bool isVisibleInitially = false, int msecsDuration = 500,
             const QEasingCurve &curve = QEasingCurve::Linear,
             QWidget *parent = nullptr);

  void fadeIn();
  void fadeOut();

  bool isFadeIn() const;
  bool isFadeOut() const;

private:
  void fade(QAbstractAnimation::Direction direction);

private:
  QGraphicsOpacityEffect *m_effect;
  QPropertyAnimation *m_animation;

  bool m_isFadeIn;
};

#endif  // FADEBUTTON_H
