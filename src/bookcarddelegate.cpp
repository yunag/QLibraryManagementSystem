#include <QPainter>

#include "bookcard.h"
#include "bookcarddelegate.h"

void BookCardDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const {
  //if (index.data().canConvert<BookCard *>()) {
  //  BookCard *bookCard = qvariant_cast<BookCard *>(index.data());

  //  bookCard->render(painter);
  //} else {
  //  QStyledItemDelegate::paint(painter, option, index);
  //}
  BookCard bookCard;
  bookCard.setBookId(index.data(Qt::UserRole).toUInt());
  bookCard.setTitle(index.data(Qt::UserRole + 1).toString());
  bookCard.setCover(index.data(Qt::UserRole + 2).value<QPixmap>());
  bookCard.setAuthors(index.data(Qt::UserRole + 3).value<QStringList>());
  bookCard.setCategories(index.data(Qt::UserRole + 4).value<QStringList>());
  bookCard.setRating(index.data(Qt::UserRole + 5).toInt());

  bookCard.render(painter->device(), QPoint(option.rect.x(), option.rect.y()),
                  QRegion(0, 0, option.rect.width(), option.rect.height()),
                  QWidget::RenderFlag::DrawChildren);
}

QSize BookCardDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const {
  if (index.data().canConvert<BookCard *>()) {
    BookCard *bookCard = qvariant_cast<BookCard *>(index.data());
    return bookCard->sizeHint();
  }
  return QStyledItemDelegate::sizeHint(option, index);
}

QWidget *BookCardDelegate::createEditor(QWidget *parent,
                                        const QStyleOptionViewItem &option,
                                        const QModelIndex &index) const

{
  //if (index.data().canConvert<StarRating>()) {
  //  StarEditor *editor = new StarEditor(parent);
  //  connect(editor, &StarEditor::editingFinished, this,
  //          &StarDelegate::commitAndCloseEditor);
  //  return editor;
  //}
  return QStyledItemDelegate::createEditor(parent, option, index);
}

void BookCardDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const {
  //if (index.data().canConvert<StarRating>()) {
  //  StarRating starRating = qvariant_cast<StarRating>(index.data());
  //  StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
  //  starEditor->setStarRating(starRating);
  //} else {
  //  QStyledItemDelegate::setEditorData(editor, index);
  //}
  QStyledItemDelegate::setEditorData(editor, index);
}

void BookCardDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const {
  //if (index.data().canConvert<StarRating>()) {
  //  StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
  //  model->setData(index, QVariant::fromValue(starEditor->starRating()));
  //} else {
  //  QStyledItemDelegate::setModelData(editor, model, index);
  //}
  //
  QStyledItemDelegate::setModelData(editor, model, index);
}
