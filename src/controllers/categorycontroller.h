#ifndef CATEGORYCONTROLLER_H
#define CATEGORYCONTROLLER_H

#include <QFuture>

#include "schema/category.h"

class CategoryController {
public:
  static QFuture<QList<Category>> getCategories();
};

#endif /* !CATEGORYCONTROLLER_H */
