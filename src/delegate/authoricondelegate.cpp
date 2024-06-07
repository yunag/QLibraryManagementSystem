#include <QApplication>
#include <QPainter>

#include "authoricondelegate.h"
#include "model/authorrestmodel.h"
#include "resourcemanager.h"

void AuthorIconDelegate::paint(QPainter *painter,
                               const QStyleOptionViewItem &option,
                               const QModelIndex &index) const {
  painter->save();

  QStyle *style =
    option.widget ? option.widget->style() : QApplication::style();

  QStyleOptionViewItem opt = option;
  initStyleOption(&opt, index);

  /* Item background */
  style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

  QRect rect = opt.rect;

  QPalette palette = opt.widget->palette();

  auto image = index.data(AuthorRestModel::ImageRole).value<QPixmap>();
  QString firstName = index.data(AuthorRestModel::FirstNameRole).toString();
  QString lastName = index.data(AuthorRestModel::LastNameRole).toString();

  QRect imageRect = rect;
  imageRect.setHeight(imageRect.height() * 8 / 10);

  if (image.isNull()) {
    /* Draw busy indicator */
    int pixmapSize = qMin(imageRect.height() / 2, imageRect.width() / 2);

    auto busyIndicator = ResourceManager::busyIndicator();
    QPixmap pixmap = busyIndicator->currentPixmap().scaled(
      pixmapSize, pixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QRect pixmapRect = pixmap.rect();
    pixmapRect.moveCenter(imageRect.center());

    painter->drawPixmap(pixmapRect.topLeft(), pixmap);
  } else {
    QPixmap pixmap = image.scaled(imageRect.size(), Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation);
    QRect pixmapRect = pixmap.rect();
    pixmapRect.moveCenter(imageRect.center());
    pixmapRect.moveBottom(imageRect.bottom());

    painter->drawPixmap(pixmapRect.topLeft(), pixmap);
  }

  QRect textRect = rect;
  textRect.setTop(imageRect.bottom());

  /* TODO: Elide very long text */
  painter->drawText(textRect,
                    Qt::AlignHCenter | Qt::AlignTop | Qt::TextWordWrap,
                    firstName + " " + lastName);

  painter->restore();
}

QSize AuthorIconDelegate::sizeHint(const QStyleOptionViewItem & /*option*/,
                                   const QModelIndex & /*index*/) const {
  return sizeHint();
}

QSize AuthorIconDelegate::sizeHint() {
  return {300, 420};
}
