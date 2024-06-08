#ifndef AUTHORCONTROLLER_H
#define AUTHORCONTROLLER_H

#include <QFuture>

#include "controller.h"
#include "schema/schema.h"

class AuthorController : public Controller {
public:
  QFuture<QList<Author>> getAuthors();
  QFuture<quint32> create(const Author &author);
  QFuture<AuthorDetails> get(quint32 id);
  QFuture<void> update(quint32 id, const Author &author);
  QFuture<void> deleteResource(quint32 id);
};

#endif /* !AUTHORCONTROLLER_H */
