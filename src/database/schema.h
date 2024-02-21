#ifndef SCHEMA_H
#define SCHEMA_H

#include <QFuture>
#include <QMetaObject>
#include <QMetaProperty>
#include <QObject>

#define SCHEMA_FIELD(type, name)                                               \
  Q_PROPERTY(type name MEMBER name)                                            \
  type name

#define SCHEMA_KEY(type, name)                                                 \
  static constexpr const char *primaryKey = #name;                             \
  SCHEMA_FIELD(type, name)

#define SCHEMA_NAME(name) static constexpr const char *schemaName = #name

class Schema {
public:
  virtual ~Schema() {}
};

#endif /* !SCHEMA_H */
