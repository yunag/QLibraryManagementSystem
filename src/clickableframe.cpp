#include <QMouseEvent>

#include "clickableframe.h"

void ClickableFrame::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::MouseButton::LeftButton) {
    emit clicked();
  }

  e->accept();
}
