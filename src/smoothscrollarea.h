#ifndef SMOOTHSCROLLAREA_H
#define SMOOTHSCROLLAREA_H

#include <QObject>
#include <QScrollArea>
#include <QWidget>

class SmoothScrollBar;

class SmoothScrollArea : public QScrollArea {
  Q_OBJECT

public:
  SmoothScrollArea(QWidget *parent = nullptr);

  void setScrollAnimation(Qt::Orientation orientation, int durationms,
                          const QEasingCurve &curve);

private:
  SmoothScrollBar *m_hScrollBar;
  SmoothScrollBar *m_vScrollBar;
};

#endif  // SMOOTHSCROLLAREA_H
