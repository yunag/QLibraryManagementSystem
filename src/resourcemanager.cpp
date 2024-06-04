#include "resourcemanager.h"
#include "libraryapplication.h"

QSharedPointer<QMovie> ResourceManager::busyIndicator() {
  QSharedPointer<QMovie> movie = instance()->m_busyIndicator.lock();
  if (movie) {
    return movie;
  }

  return instance()->m_busyIndicator = QSharedPointer<QMovie>(
           new QMovie(":/images/LoadingSpinner.gif"), [](QMovie *movie) {
    movie->stop();
    movie->deleteLater();
  });
}

ResourceManager *ResourceManager::instance() {
  return App->resourceManager();
}

RestApiManager *ResourceManager::networkManager() {
  return &instance()->m_networkManager;
}
