#ifndef AUTHORCONTROLLER_H
#define AUTHORCONTROLLER_H

#include <QFuture>

#include "schema/author.h"

class AuthorController {
public:
  static QFuture<QList<Author>> getAuthors();
};

#endif /* !AUTHORCONTROLLER_H */
