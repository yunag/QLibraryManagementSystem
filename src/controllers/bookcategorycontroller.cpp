#include "bookcategorycontroller.h"
#include "resourcemanager.h"

QFuture<QByteArray> BookCategoryController::createRelation(quint32 bookId,
                                                           quint32 categoryId) {
  RestApiManager *manager = ResourceManager::networkManager();

  QString apiRoute = "/api/books/%1/categories/%2";

  auto [future, reply] = manager->post(apiRoute.arg(bookId).arg(categoryId));

  return future;
}

QFuture<QByteArray> BookCategoryController::deleteRelation(quint32 bookId,
                                                           quint32 categoryId) {
  RestApiManager *manager = ResourceManager::networkManager();

  QString apiRoute = "/api/books/%1/categories/%2";

  auto [future, reply] =
    manager->deleteResource(apiRoute.arg(bookId).arg(categoryId));
  return future;
}

QFuture<QByteArray>
BookCategoryController::updateRelations(quint32 bookId,
                                        const QList<quint32> &categoriesId) {
  RestApiManager *manager = ResourceManager::networkManager();

  QString apiRoute = "/api/books/%1/categories";

  QVariantMap values;
  values["ids"] = QVariant::fromValue(categoriesId).toList();

  auto [future, reply] = manager->put(apiRoute.arg(bookId), values);

  return future;
}
