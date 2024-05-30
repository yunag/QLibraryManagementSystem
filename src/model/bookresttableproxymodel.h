#ifndef BOOKRESTTABLEPROXYMODEL_H
#define BOOKRESTTABLEPROXYMODEL_H

#include <QIdentityProxyModel>

class BookRestTableProxyModel : public QIdentityProxyModel {
public:
  using QIdentityProxyModel::QIdentityProxyModel;

  enum HorizontalSection {
    Id = 0,
    Title,
    Authors,
    Categories,
    Rating,
    LastSection = Rating
  };

  Q_ENUM(HorizontalSection);

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  static QVariant dataForColumn(int col, const QModelIndex &sourceIndex);
  QModelIndex index(int row, int column,
                    const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &proxyIndex,
                int role = Qt::DisplayRole) const override;
};

#endif /* !BOOKRESTTABLEPROXYMODEL_H */
