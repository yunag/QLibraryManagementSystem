#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <QFuture>
#include <QPixmap>

#include <QUrl>

#include "network/network.h"

class ImageLoader {
public:
  struct ImageReply {
    QFuture<QPixmap> future;
    ReplyPointer reply;
  };

  using FutureImage = ImageReply;

public:
  ImageLoader(RestApiManager *manager);

  FutureImage load(const QUrl &url);

private:
  RestApiManager *m_manager;
};

#endif /* !IMAGELOADER_H */
