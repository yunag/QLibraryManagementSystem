#ifndef DRAGDROPLISTVIEW_H
#define DRAGDROPLISTVIEW_H

#include <QListView>

class DragDropListView : public QListView {
  Q_OBJECT
public:
  explicit DragDropListView(QWidget *parent = nullptr);

  void acceptDropsFrom(QObject *object);

protected:
  void dragMoveEvent(QDragMoveEvent *e) override;

private:
  QSet<QObject *> m_acceptDrops;
};

#endif  // DRAGDROPLISTVIEW_H
