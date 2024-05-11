#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>

class QJsonObject;

struct Category {
  quint32 id;
  QString name;

  static Category fromJson(const QJsonObject &json);
};

#endif  // CATEGORY_H
