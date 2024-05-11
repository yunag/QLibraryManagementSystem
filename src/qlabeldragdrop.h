#ifndef QLABELDRAGDROP_H
#define QLABELDRAGDROP_H

#include <QLabel>
#include <QObject>

class QLabelDragDrop : public QLabel {
  Q_OBJECT
public:
  explicit QLabelDragDrop(QWidget *parent = nullptr);

protected:
  void dropEvent(QDropEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragLeaveEvent(QDragLeaveEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
};

#endif  // QLABELDRAGDROP_H
