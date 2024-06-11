#include <QNetworkReply>

#include "network/network.h"

#include "network/imageloader.h"

using FutureImage = ImageLoader::FutureImage;

ImageLoader::ImageLoader(RestApiManager *manager) : m_manager(manager) {}

FutureImage ImageLoader::load(const QUrl &url) {
  QNetworkRequest request(url);

  auto [future, reply] = m_manager->get(request);

  auto futureImage =
    future.then(QtFuture::Launch::Async, [](const QByteArray &data) {
    QPixmap pixmap;
    pixmap.loadFromData(data);

    return pixmap;
  });

  return {futureImage, reply};
}
