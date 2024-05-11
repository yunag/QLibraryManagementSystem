#include <QMouseEvent>

#include "clickableframe.h"

void ClickableFrame::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    emit clicked();
  }

  event->accept();
}
