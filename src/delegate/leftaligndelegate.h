#ifndef LEFTALIGNDELEGATE_H
#define LEFTALIGNDELEGATE_H

#include <QStyledItemDelegate>

class LeftAlignDelegate : public QStyledItemDelegate {
public:
  using QStyledItemDelegate::QStyledItemDelegate;

protected:
  void initStyleOption(QStyleOptionViewItem *option,
                       const QModelIndex &index) const override;
};

#endif /* !LEFTALIGNDELEGATE_H */
