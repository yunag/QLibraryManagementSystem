#ifndef AUTHOR_H
#define AUTHOR_H

#include <QString>
#include <QUrl>

class QHttpMultiPart;
class QJsonObject;

struct Author {
  quint32 id;
  QString firstName;
  QString lastName;
  QUrl imageUrl;

  static Author fromJson(const QJsonObject &json);
  QHttpMultiPart *createHttpMultiPart() const;
};

#endif  // AUTHOR_H
