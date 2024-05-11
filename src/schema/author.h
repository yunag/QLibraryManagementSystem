#ifndef AUTHOR_H
#define AUTHOR_H

#include <QList>
#include <QString>

class QJsonObject;

struct Author {
  quint32 id;
  QString firstName;
  QString lastName;

  static Author fromJson(const QJsonObject &json);
};

#endif  // AUTHOR_H
