#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QCache>
#include <QMovie>
#include <QString>

#include <memory>

template <typename T>
using CacheType = QCache<QString, std::shared_ptr<T>>;

class ResourceManager {
  ResourceManager() {}

private:
  CacheType<QMovie> m_movies;
  CacheType<QPixmap> m_pixmaps;
};

#endif /* !RESOURCEMANAGER_H */
