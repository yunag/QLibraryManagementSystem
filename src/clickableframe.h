#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <QFrame>

class ClickableFrame : public QFrame {
  Q_OBJECT
  using QFrame::QFrame;

protected:
  void mousePressEvent(QMouseEvent *event) override;

signals:
  void clicked();
};

#endif  // CLICKABLEFRAME_H
