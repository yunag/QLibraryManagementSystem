#ifndef AUTHORDETAILSDAO_H
#define AUTHORDETAILSDAO_H

#include <QFuture>
#include <QObject>

#include "author.h"

struct AuthorDetails {
  Author author;

  struct BookInfo {
    QString title;
    QString coverPath;
    quint32 id;
  };

  QList<BookInfo> books;
};

class AuthorDetailsDAO : public QObject {
  Q_OBJECT
public:
  explicit AuthorDetailsDAO(QObject *parent = nullptr);

  QFuture<AuthorDetails> fetchDetails(quint32 authorId);

private:
  static const QString kQuery;
};

#endif  // AUTHORDETAILSDAO_H
