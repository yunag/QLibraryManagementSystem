#ifndef USER_H
#define USER_H

#include <QDate>
#include <QFuture>

#include "database/schema.h"

class User : public Schema {
  Q_GADGET

public:
  SCHEMA_KEY(quint32, user_id);

  SCHEMA_FIELD(QString, name);
  SCHEMA_FIELD(QString, username);
  SCHEMA_FIELD(QString, password);
  SCHEMA_FIELD(QString, salt);

  SCHEMA_FIELD(QDate, joined_date);

  SCHEMA_FIELD(bool, is_admin);
  SCHEMA_FIELD(bool, active);

  static QByteArray generateSalt();
  static QFuture<bool> validate(const QString &username,
                                const QString &password);
};

#endif  // USER_H
