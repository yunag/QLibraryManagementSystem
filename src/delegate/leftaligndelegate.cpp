#include "leftaligndelegate.h"

void LeftAlignDelegate::initStyleOption(QStyleOptionViewItem *option,
                                        const QModelIndex &index) const {
  QStyledItemDelegate::initStyleOption(option, index);
  option->decorationPosition = QStyleOptionViewItem::Left;
}
