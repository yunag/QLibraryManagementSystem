#include <QMovie>

#include "network/imageloader.h"
#include "network/network.h"

#include "resourcemanager.h"
#include "widgetutils.h"

ReplyPointer WidgetUtils::asyncLoadImage(AspectRatioLabel *label,
                                         const QUrl &url) {
  QPixmap notFoundPixmap(":/images/DefaultBookCover");

  if (!url.isValid() || url.isEmpty()) {
    label->setPixmap(notFoundPixmap);
    return nullptr;
  }

  auto busyIndicator = ResourceManager::busyIndicator();
  busyIndicator->start();

  QMovie *busyIndicatorRaw = busyIndicator.get();

  QObject::connect(busyIndicatorRaw, &QMovie::frameChanged, label,
                   [busyIndicatorRaw, label]() {
    label->setPixmap(busyIndicatorRaw->currentPixmap());
  });

  ImageLoader imageLoader(ResourceManager::networkManager());

  auto [future, reply] = imageLoader.load(url);

  future
    .then(label,
          [label, busyIndicator](const QPixmap &image) {
    QObject::disconnect(busyIndicator.get(), &QMovie::frameChanged, label,
                        nullptr);
    label->setPixmap(image);
  })
    .onFailed(label, [label, notFoundPixmap = std::move(notFoundPixmap),
                      busyIndicator](const NetworkError &err) {
    QObject::disconnect(busyIndicator.get(), &QMovie::frameChanged, label,
                        nullptr);
    if (err.type() != QNetworkReply::OperationCanceledError) {
      label->setPixmap(notFoundPixmap);
    }
  });

  return reply;
}
