#ifndef CATEGORY_H
#define CATEGORY_H

#include <QFuture>
#include <QtGlobal>

#include "schema.h"

class Category : public Schema {
  Q_GADGET

public:
  SCHEMA_NAME(category);
  SCHEMA_KEY(quint32, category_id);
  SCHEMA_FIELD(QString, name);
};

#endif  // CATEGORY_H
