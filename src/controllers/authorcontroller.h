#ifndef AUTHORCONTROLLER_H
#define AUTHORCONTROLLER_H

#include <QFuture>

#include "schema/author.h"

class AuthorController {
public:
  static QFuture<QList<Author>> getAuthors();
  static QFuture<quint32> createAuthor(const Author &author);
};

#endif /* !AUTHORCONTROLLER_H */
