#ifndef AUTHORBOOKCONTROLLER_H
#define AUTHORBOOKCONTROLLER_H

#include <QFuture>

class AuthorBookController {
public:
  static QFuture<QByteArray> createRelation(quint32 authorId, quint32 bookId);
  static QFuture<QByteArray> deleteRelation(quint32 authorId, quint32 bookId);
  static QFuture<QByteArray> updateRelations(quint32 bookId,
                                             const QList<quint32> &authorsId);
};

#endif /* !AUTHORBOOKCONTROLLER_H */
