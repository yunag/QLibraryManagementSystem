#ifndef QLABELDRAGDROP_H
#define QLABELDRAGDROP_H

#include <QLabel>
#include <QObject>

class QLabelDragDrop : public QLabel {
  Q_OBJECT
public:
  explicit QLabelDragDrop(QWidget *parent = nullptr);

protected:
  virtual void dropEvent(QDropEvent *event) override;
  virtual void dragEnterEvent(QDragEnterEvent *event) override;
  virtual void dragLeaveEvent(QDragLeaveEvent *event) override;
  virtual void dragMoveEvent(QDragMoveEvent *event) override;
};

#endif  // QLABELDRAGDROP_H
