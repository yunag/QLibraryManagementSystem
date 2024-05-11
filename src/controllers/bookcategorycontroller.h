#ifndef BOOKCATEGORYCONTROLLER_H
#define BOOKCATEGORYCONTROLLER_H

#include <QFuture>

class BookCategoryController {
public:
  static QFuture<QByteArray> createRelation(quint32 bookId, quint32 categoryId);
  static QFuture<QByteArray> deleteRelation(quint32 bookId, quint32 categoryId);
  static QFuture<QByteArray>
  updateRelations(quint32 bookId, const QList<quint32> &categoriesId);
};

#endif /* !BOOKCATEGORYCONTROLLER_H */
