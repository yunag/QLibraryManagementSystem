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

  QSharedPointer<QMovie> movie = App->loadingMovie();
  label->setMovie(movie.get());
  movie->start();

  RestApiManager *networkManager = App->network();
  ImageLoader imageLoader(networkManager);

  auto [future, reply] = imageLoader.load(url);

  future
    .then(label,
          [label, movie](const QPixmap &image) {
            label->setMovie(nullptr);
            label->setPixmap(image);
          })
    .onFailed(label, [label, notFoundPixmap = std::move(notFoundPixmap),
                      movie](const NetworkError &err) {
      label->setMovie(nullptr);
      if (err.type() != QNetworkReply::OperationCanceledError) {
        label->setPixmap(notFoundPixmap);
      }
    });

  return reply;
}
