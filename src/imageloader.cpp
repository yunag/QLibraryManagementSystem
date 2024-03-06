#include <QDebug>

#include "imageloader.h"

QPixmap ImageLoader::load(QString imagePath) {
  QPixmap pixmap;

  if (!pixmap.load(imagePath) &&
      !pixmap.load(":/resources/images/DefaultBookCover")) {
    qWarning() << "Couldn't load images";
    return QPixmap();
  }

  return pixmap;
}
