#include <QDragMoveEvent>
#include <QSet>

#include "dragdroplistview.h"

DragDropListView::DragDropListView(QWidget *parent) : QListView(parent) {}

void DragDropListView::dragMoveEvent(QDragMoveEvent *e) {
  if (m_acceptDrops.contains(e->source())) {
    e->accept();
  } else {
    e->ignore();
  }
}

void DragDropListView::acceptDropsFrom(QObject *object) {
  m_acceptDrops.insert(object);
}
