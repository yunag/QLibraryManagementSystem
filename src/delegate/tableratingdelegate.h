#ifndef TABLERATINGDELEGATE_H
#define TABLERATINGDELEGATE_H

#include <QStyledItemDelegate>

#include "kratingpainter.h"

class TableRatingDelegate : public QStyledItemDelegate {
public:
  TableRatingDelegate(QObject *parent = nullptr);

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;

  bool editorEvent(QEvent *event, QAbstractItemModel *model,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) override;

private:
  KRatingPainter m_painter;
};

#endif /* !TABLERATINGDELEGATE_H */
