#ifndef CLICKABLEFRAME_H
#define CLICKABLEFRAME_H

#include <QFrame>

class ClickableFrame : public QFrame {
  Q_OBJECT
  using QFrame::QFrame;

protected:
  virtual void mousePressEvent(QMouseEvent *e) override;

signals:
  void clicked();
};

#endif  // CLICKABLEFRAME_H
