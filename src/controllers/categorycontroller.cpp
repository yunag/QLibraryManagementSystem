#include <QJsonArray>
#include <QJsonObject>

#include "common/json.h"
#include "network/network.h"

#include "categorycontroller.h"
#include "libraryapplication.h"

QFuture<QList<Category>> CategoryController::getCategories() {
  RestApiManager *manager = App->network();

  auto [future, reply] = manager->get("/api/categories");

  return future.then([](const QByteArray &data) {
    QJsonArray json = json::byteArrayToJson(data)->array();

    QList<Category> categories;
    for (const auto categoryJson : json) {
      Category category = Category::fromJson(categoryJson.toObject());

      categories.push_back(category);
    }
    return categories;
  });
}
