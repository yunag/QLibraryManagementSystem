#ifndef BOOKCARDDELEGATE_H
#define BOOKCARDDELEGATE_H

#include <QStandardItem>
#include <QStyledItemDelegate>

class BookCard;

class BookCardItem : public QStandardItem {
  using QStandardItem::QStandardItem;

  void setBookCard();

private:
  BookCard *bookCard;
};

class BookCardDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  using QStyledItemDelegate::QStyledItemDelegate;

  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const override;
  QSize sizeHint(const QStyleOptionViewItem &option,
                 const QModelIndex &index) const override;
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const override;
  void setEditorData(QWidget *editor, const QModelIndex &index) const override;
  void setModelData(QWidget *editor, QAbstractItemModel *model,
                    const QModelIndex &index) const override;
};

#endif  // BOOKCARDDELEGATE_H
