#include "qlabeldragdrop.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

QLabelDragDrop::QLabelDragDrop(QWidget *parent) : QLabel(parent) {
  setAcceptDrops(true);
  setMouseTracking(true);
}

void QLabelDragDrop::dragEnterEvent(QDragEnterEvent *event) {
  qDebug() << "dragEnterEvent";
  event->acceptProposedAction();
}

void QLabelDragDrop::dragLeaveEvent(QDragLeaveEvent *event) {
  qDebug() << "dragLeaveEvent";
  releaseMouse();
}

void QLabelDragDrop::dragMoveEvent(QDragMoveEvent *event) {
  qDebug() << "dragMoveEvent";
}

void QLabelDragDrop::dropEvent(QDropEvent *event) { qDebug() << "dropEvent"; }
