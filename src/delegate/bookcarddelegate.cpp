#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include "bookcard.h"
#include "bookcarddelegate.h"

#include "model/bookrestmodel.h"

void BookCardDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const {
  if (index.data(BookRestModel::ObjectRole).canConvert<BookCard>()) {
    auto bookCard = index.data(BookRestModel::ObjectRole).value<BookCard>();

    int hoverRating = index.data(BookRestModel::HoverRatingRole).toInt();
    bookCard.paint(painter, option, hoverRating);
  } else {
    QStyledItemDelegate::paint(painter, option, index);
  }
}

QSize BookCardDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const {
  if (index.data(BookRestModel::ObjectRole).canConvert<BookCard>()) {
    auto bookCard = index.data(BookRestModel::ObjectRole).value<BookCard>();
    return bookCard.sizeHint();
  }

  return QStyledItemDelegate::sizeHint(option, index);
}

bool BookCardDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) {
  if (!index.data(BookRestModel::ObjectRole).canConvert<BookCard>()) {
    return QStyledItemDelegate::editorEvent(event, model, option, index);
  }

  QRect rect = option.rect;
  const auto &bookCard =
    index.data(BookRestModel::ObjectRole).value<BookCard>();

  auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
  if (!mouseEvent) {
    return false;
  }

  QStyle::State state = QStyle::State_Enabled;

  if (bookCard.buttonRect(rect).contains(mouseEvent->pos())) {
    state |= QStyle::State_MouseOver | QStyle::State_Active;

    if (event->type() == QEvent::MouseButtonPress) {
      state |= QStyle::State_Sunken;
    } else if (event->type() == QEvent::MouseButtonRelease) {
      copyButtonClicked(index.data(BookRestModel::TitleRole).toString());
    }
  }

  if (model->setData(index, QVariant::fromValue(state),
                     BookRestModel::ButtonStateRole)) {
    return true;
  }

  if (event->type() == QEvent::MouseMove) {
    int rating = bookCard.ratingFromPosition(rect, mouseEvent->pos());
    return model->setData(index, rating, BookRestModel::HoverRatingRole);
  }

  if (event->type() == QEvent::MouseButtonPress) {
    int rating = bookCard.ratingFromPosition(rect, mouseEvent->pos());
    if (rating != -1) {
      return model->setData(index, rating, BookRestModel::RatingRole);
    }
  }

  return false;
}

void BookCardDelegate::copyButtonClicked(const QString &text) {
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
}
