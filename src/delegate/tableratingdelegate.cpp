#include <QMouseEvent>

#include "model/bookrestmodel.h"
#include "tableratingdelegate.h"

TableRatingDelegate::TableRatingDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {
  m_painter.setIcon(QIcon(":/images/starRating.svg"));
}

void TableRatingDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {
  int rating = index.data(BookRestModel::RatingRole).toInt();
  int hoverRating = index.data(BookRestModel::HoverRatingRole).toInt();
  m_painter.paint(painter, option.rect, rating, hoverRating);
}

bool TableRatingDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                      const QStyleOptionViewItem &option,
                                      const QModelIndex &index) {
  auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
  if (!mouseEvent) {
    return false;
  }

  QRect rect = option.rect;

  if (event->type() == QEvent::MouseMove) {
    int rating = m_painter.ratingFromPosition(rect, mouseEvent->pos());
    return model->setData(index, rating, BookRestModel::HoverRatingRole);
  }

  if (event->type() == QEvent::MouseButtonPress) {
    int rating = m_painter.ratingFromPosition(rect, mouseEvent->pos());
    if (rating != -1) {
      return model->setData(index, rating, BookRestModel::RatingRole);
    }
  }
  return false;
}
