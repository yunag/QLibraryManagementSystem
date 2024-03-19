#ifndef AUTHOR_H
#define AUTHOR_H

#include "schema.h"

class Author : public Schema {
  Q_GADGET

public:
  SCHEMA_NAME(author);
  SCHEMA_KEY(quint32, author_id);
  SCHEMA_FIELD(QString, first_name);
  SCHEMA_FIELD(QString, last_name);
};

#endif  // AUTHOR_H
