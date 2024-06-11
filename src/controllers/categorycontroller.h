#ifndef CATEGORYCONTROLLER_H
#define CATEGORYCONTROLLER_H

#include <QFuture>

#include "controllers/controller.h"
#include "schema/category.h"

class CategoryController : public Controller {
public:
  QFuture<QList<Category>> getCategories();
  QFuture<void> create(const Category &category);
  QFuture<void> deleteResource(quint32 id);
};

#endif /* !CATEGORYCONTROLLER_H */
