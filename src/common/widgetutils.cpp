#include <QMovie>

#include "network/imageloader.h"
#include "network/network.h"

#include "libraryapplication.h"
#include "widgetutils.h"

ReplyPointer WidgetUtils::asyncLoadImage(AspectRatioLabel *label,
                                         const QUrl &url) {
  QPixmap notFoundPixmap(":/images/DefaultBookCover");

  if (!url.isValid() || url.isEmpty()) {
    label->setPixmap(notFoundPixmap);
    return nullptr;
  }

  QSharedPointer<QMovie> movie = App->busyIndicator();
  movie->start();

  QMovie *movieRaw = movie.get();

  QObject::connect(movieRaw, &QMovie::frameChanged, label, [movieRaw, label]() {
    label->setPixmap(movieRaw->currentPixmap());
  });

  RestApiManager *networkManager = App->network();
  ImageLoader imageLoader(networkManager);

  auto [future, reply] = imageLoader.load(url);

  future
    .then(label,
          [label, movie](const QPixmap &image) {
    QObject::disconnect(movie.get(), &QMovie::frameChanged, label, nullptr);
    label->setPixmap(image);
  })
    .onFailed(label, [label, notFoundPixmap = std::move(notFoundPixmap),
                      movie](const NetworkError &err) {
    QObject::disconnect(movie.get(), &QMovie::frameChanged, label, nullptr);
    if (err.type() != QNetworkReply::OperationCanceledError) {
      label->setPixmap(notFoundPixmap);
    }
  });

  return reply;
}
