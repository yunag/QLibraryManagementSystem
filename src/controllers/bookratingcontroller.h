#ifndef BOOKRATINGCONTROLLER_H
#define BOOKRATINGCONTROLLER_H

#include <QFuture>

class BookRatingController {
public:
  static QFuture<QByteArray> rateBook(quint32 bookId, int rating);
};

#endif /* !BOOKRATINGCONTROLLER_H */
