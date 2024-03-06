#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QPixmap>

class ImageLoader {
public:
  static QPixmap load(QString imagePath);
};

#endif /* !IMAGELOADER_H */
