#ifndef AUTHORRESTMODEL_H
#define AUTHORRESTMODEL_H

#include <QMovie>
#include <QPixmap>

#include "abstractrestmodel.h"
#include "schema/author.h"

struct AuthorItem : public Author {
  AuthorItem(Author author) : Author(std::move(author)) {};
  QPixmap image;
  Qt::ItemFlags flags = {Qt::ItemIsEnabled | Qt::ItemIsSelectable};
};

class AuthorRestModel : public AbstractRestModel {
  Q_OBJECT

public:
  explicit AuthorRestModel(QObject *parent = nullptr);

  enum HorizontalSection {
    IdHeader = 0,
    FirstNameHeader,
    LastNameHeader,
    LastHeader = LastNameHeader
  };

  Q_ENUM(HorizontalSection);

  enum AuthorsRole {
    TitleRole = Qt::UserRole + 1,
    ObjectRole,
    IdRole,
    ImageUrlRole,
    ImageRole,
    FirstNameRole,
    LastNameRole,
    FlagsRole
  };
  Q_ENUM(AuthorsRole)

  void handleRequestData(const QByteArray &data) override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const override;
  static QVariant dataForColumn(const QModelIndex &index);

  int columnCount(const QModelIndex &parent) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;

  bool canFetchMore(const QModelIndex &parent) const override;
  void fetchMore(const QModelIndex &parent) override;

  void shouldFetchImages(bool shouldFetchImages);

  void reset();

  Qt::ItemFlags flags(const QModelIndex &index) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role) override;

private:
  QList<AuthorItem> m_authors;

  bool m_shouldFetchImages;
  int m_authorsCount;
};

#endif /* !AUTHORRESTMODEL_H */
