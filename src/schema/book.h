#ifndef BOOK_H
#define BOOK_H

#include <QDate>
#include <QUrl>

class QHttpMultiPart;

struct Book {
  quint32 id = 0;
  QString title;
  QString description;
  QDate publicationDate;
  QUrl coverUrl;
  qreal rating;
  int copiesOwned = 0;

  static Book fromJson(const QJsonObject &json);
  QHttpMultiPart *createHttpMultiPart() const;
};

#endif  // BOOK_H
