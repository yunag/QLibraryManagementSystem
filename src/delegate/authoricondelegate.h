#ifndef AUTHORICONDELEGATE_H
#define AUTHORICONDELEGATE_H

#include <QStyledItemDelegate>

class AuthorIconDelegate : public QStyledItemDelegate {
public:
  using QStyledItemDelegate::QStyledItemDelegate;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;

  static QSize sizeHint();
};

#endif /* !AUTHORICONDELEGATE_H */
