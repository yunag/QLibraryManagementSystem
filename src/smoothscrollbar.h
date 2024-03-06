#ifndef SMOOTHSCROLLBAR_H
#define SMOOTHSCROLLBAR_H

#include <QObject>
#include <QScrollBar>

class QPropertyAnimation;
class SmoothScrollArea;

class SmoothScrollBar : public QScrollBar {
  Q_OBJECT

public:
  explicit SmoothScrollBar(QWidget *parent = nullptr);
  SmoothScrollBar(QEasingCurve curve, int duration, QWidget *parent = nullptr);

  void scrollValue(int value);
  void setAnimationDuration(int msecs);
  void setAnimationCurve(QEasingCurve curve);

signals:
  void scrollFinished();

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

protected:
  QPropertyAnimation *m_animation;
};

#endif  // SMOOTHSCROLLBAR_H
