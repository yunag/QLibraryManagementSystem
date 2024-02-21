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

  static QFuture<void> update(quint32 book_id, QList<quint32> category_id);
};

#endif  // CATEGORY_H
