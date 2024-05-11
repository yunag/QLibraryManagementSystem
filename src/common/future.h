#ifndef COMMONFUTURE_H
#define COMMONFUTURE_H

#include <QFutureSynchronizer>
#include <QSharedPointer>

template <typename T, typename Syncronizer = QFutureSynchronizer<T>>
QSharedPointer<Syncronizer> makeFutureSyncronizer() {
  return QSharedPointer<Syncronizer>(new Syncronizer,
                                     [](Syncronizer *syncronizer) {
                                       syncronizer->clearFutures();
                                       delete syncronizer;
                                     });
}

#endif /* !COMMONFUTURE_H */
