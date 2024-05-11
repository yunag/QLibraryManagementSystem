#include <QDragMoveEvent>
#include <QSet>

#include "dragdroplistview.h"

DragDropListView::DragDropListView(QWidget *parent) : QListView(parent) {}

void DragDropListView::dragMoveEvent(QDragMoveEvent *event) {
  if (m_acceptDrops.contains(event->source())) {
    event->accept();
  } else {
    event->ignore();
  }
}

void DragDropListView::acceptDropsFrom(QObject *object) {
  m_acceptDrops.insert(object);
}
