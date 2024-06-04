#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QMovie>

#include "network/network.h"

class LibraryApplication;

class ResourceManager {
  friend LibraryApplication;

public:
  static QSharedPointer<QMovie> busyIndicator();
  static RestApiManager *networkManager();

  static ResourceManager *instance();

private:
  ResourceManager() = default;

  QWeakPointer<QMovie> m_busyIndicator;
  RestApiManager m_networkManager;
};

#endif /* !RESOURCEMANAGER_H */
