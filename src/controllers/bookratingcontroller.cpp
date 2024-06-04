#include "controllers/bookratingcontroller.h"
#include "resourcemanager.h"

#include "network/network.h"

QFuture<QByteArray> BookRatingController::rateBook(quint32 bookId, int rating) {
  RestApiManager *manager = ResourceManager::networkManager();

  QString apiRoute = "/api/books/%1/rating";
  QVariantMap values;
  values["rating"] = rating;

  auto [future, reply] = manager->put(apiRoute.arg(bookId), values);

  return future;
}
