#ifndef AUTHOR_H
#define AUTHOR_H

#include <QString>
#include <QUrl>

class QJsonObject;

struct Author {
  quint32 id;
  QString firstName;
  QString lastName;
  QUrl imageUrl;

  static Author fromJson(const QJsonObject &json);
};

#endif  // AUTHOR_H
