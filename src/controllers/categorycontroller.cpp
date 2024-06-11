#include <QJsonArray>
#include <QJsonObject>

#include "common/algorithm.h"
#include "common/json.h"
#include "network/network.h"

#include "categorycontroller.h"

QFuture<QList<Category>> CategoryController::getCategories() {
  auto [future, reply] = m_manager->get("/api/categories");

  return future.then([](const QByteArray &data) {
    QJsonArray json = json::byteArrayToJson(data)->array();

    return algorithm::transform<QList<Category>>(json,
                                                 [](const auto &categoryJson) {
      return Category::fromJson(categoryJson.toObject());
    });
  });
}

QFuture<void> CategoryController::create(const Category &category) {
  QVariantMap values;
  values["name"] = category.name;

  auto [future, reply] = m_manager->post("/api/categories", values);

  return future.then([](auto) {});
}

QFuture<void> CategoryController::deleteResource(quint32 id) {
  auto [future, reply] =
    m_manager->deleteResource("/api/categories/" + QString::number(id));

  return future.then([](auto) {});
}
