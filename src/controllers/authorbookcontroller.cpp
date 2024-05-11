#include "authorbookcontroller.h"
#include "libraryapplication.h"

QFuture<QByteArray> AuthorBookController::createRelation(quint32 authorId,
                                                         quint32 bookId) {
  RestApiManager *manager = App->network();

  QString apiRoute = "/api/authors/%1/books/%2";

  auto [future, reply] = manager->post(apiRoute.arg(authorId).arg(bookId));

  return future;
}

QFuture<QByteArray> AuthorBookController::deleteRelation(quint32 authorId,
                                                         quint32 bookId) {
  RestApiManager *manager = App->network();

  QString apiRoute = "/api/authors/%1/books/%2";

  auto [future, reply] =
    manager->deleteResource(apiRoute.arg(authorId).arg(bookId));

  return future;
}

QFuture<QByteArray>
AuthorBookController::updateRelations(quint32 bookId,
                                      const QList<quint32> &authorsId) {
  RestApiManager *manager = App->network();

  QString apiRoute = "/api/books/%1/authors";

  QVariantMap values;
  values["ids"] = QVariant::fromValue(authorsId).toList();

  auto [future, reply] = manager->put(apiRoute.arg(bookId), values);

  return future;
}
