#include <QPersistentModelIndex>

#include "libraryapplication.h"
#include "loadingmodel.h"

#include "network/imageloader.h"
#include "network/network.h"

LoadingModel::LoadingModel(QObject *parent) : QStandardItemModel(parent) {}

QVariant LoadingModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return {};

  switch (role) {
    case Qt::DecorationRole:
      if (m_data.find(index) != m_data.end()) {
        return m_data[index].movie->currentPixmap();
      }
    default:
      return QStandardItemModel::data(index, role);
  }
}

void LoadingModel::appendRow(QStandardItem *item, const QUrl &url,
                             QSharedPointer<QMovie> movie) {
  QStandardItemModel::appendRow(item);

  QPersistentModelIndex persistentIndex = item->index();

  RestApiManager *manager = App->network();
  ImageLoader imageLoader(manager);

  movie->start();

  connect(
    movie.get(), &QMovie::frameChanged, this, [this, persistentIndex](int) {
      emit dataChanged(persistentIndex, persistentIndex, {Qt::DecorationRole});
    });

  auto [future, reply] = imageLoader.load(url);

  Data data{movie, future, reply};

  m_data.insert(persistentIndex, data);

  future.then(this, [item](const QPixmap &image) { item->setIcon(image); })
    .onFailed(this,
              [item] { item->setIcon(QPixmap(":/images/DefaultBookCover")); })
    .then(this, [this, persistentIndex, movie]() {
      auto it = m_data.find(persistentIndex);
      if (it != m_data.end()) {
        m_data.erase(it);
      }
    });
}

void LoadingModel::clear() {
  for (auto &data : m_data) {
    data.future.cancel();
    data.reply->abort();
  }

  m_data.clear();

  QStandardItemModel::clear();
}
