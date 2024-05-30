#ifndef BOOKCARDDELEGATE_H
#define BOOKCARDDELEGATE_H

#include <QStyledItemDelegate>

class BookCardDelegate : public QStyledItemDelegate {
  Q_OBJECT

public:
  using QStyledItemDelegate::QStyledItemDelegate;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;

  bool editorEvent(QEvent *event, QAbstractItemModel *model,
                   const QStyleOptionViewItem &option,
                   const QModelIndex &index) override;

private:
  static void copyButtonClicked(const QString &text);
};

#endif  // BOOKCARDDELEGATE_H
