#include <QApplication>
#include <QClipboard>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

#include "bookcard.h"
#include "bookcarddelegate.h"

#include "bookrestmodel.h"

void BookCardDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const {
  if (index.data().canConvert<BookCard>()) {
    auto bookCard = qvariant_cast<BookCard>(index.data());

    bookCard.paint(painter, option);
  } else {
    QStyledItemDelegate::paint(painter, option, index);
  }
}

QSize BookCardDelegate::sizeHint(const QStyleOptionViewItem &option,
                                 const QModelIndex &index) const {
  if (index.data().canConvert<BookCard>()) {
    auto bookCard = qvariant_cast<BookCard>(index.data());
    return bookCard.sizeHint();
  }

  return QStyledItemDelegate::sizeHint(option, index);
}

bool BookCardDelegate::editorEvent(QEvent *event, QAbstractItemModel *model,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) {
  if (!index.data().canConvert<BookCard>()) {
    return QStyledItemDelegate::editorEvent(event, model, option, index);
  }

  QRect rect = option.rect;
  auto bookCard = qvariant_cast<BookCard>(index.data());

  auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);
  if (!mouseEvent) {
    return false;
  }

  if (bookCard.buttonRect(rect).contains(mouseEvent->pos())) {
    QStyle::State state =
      QStyle::State_Enabled | QStyle::State_Active | QStyle::State_MouseOver;

    if (event->type() == QEvent::MouseButtonPress) {
      state |= QStyle::State_Sunken;
    } else if (event->type() == QEvent::MouseButtonRelease) {
      copyButtonClicked(index.data(BookRestModel::TitleRole).toString());
    }

    model->setData(index, QVariant::fromValue(state),
                   BookRestModel::ButtonStateRole);

    return true;
  }

  if (event->type() == QEvent::MouseMove) {
    int rating = bookCard.ratingFromPosition(rect, mouseEvent->pos());
    model->setData(index, rating, BookRestModel::HoverRatingRole);

    return true;
  }

  if (event->type() == QEvent::MouseButtonPress) {
    int rating = bookCard.ratingFromPosition(rect, mouseEvent->pos());
    if (rating != -1) {
      model->setData(index, rating, BookRestModel::RatingRole);
      return true;
    }
  }

  return false;
}

void BookCardDelegate::copyButtonClicked(const QString &text) {
  QClipboard *clipboard = QApplication::clipboard();
  clipboard->setText(text);
}
