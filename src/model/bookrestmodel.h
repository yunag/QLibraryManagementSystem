#ifndef BOOKRESTMODEL_H
#define BOOKRESTMODEL_H

#include <QAbstractListModel>

#include "schema/schema.h"

#include "abstractrestmodel.h"
#include "bookcard.h"

class BookRestModel : public AbstractRestModel {
  Q_OBJECT

public:
  explicit BookRestModel(QObject *parent = nullptr);

  enum HorizontalSection {
    IdHeader = 0,
    TitleHeader,
    AuthorsHeader,
    CategoriesHeader,
    RatingHeader,
    LastHeader = RatingHeader
  };

  Q_ENUM(HorizontalSection);

  enum BookCardRoles {
    TitleRole = Qt::UserRole + 1,
    ObjectRole,
    IdRole,
    CoverRole,
    CoverUrlRole,
    AuthorsRole,
    CategoriesRole,
    RatingRole,
    ButtonStateRole,
    HoverRatingRole
  };
  Q_ENUM(BookCardRoles)

  void handleRequestData(const QByteArray &data) override;
  QUrlQuery includeFieldsQuery() override;
  void reset();

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const override;
  static QVariant dataForColumn(const QModelIndex &index);

  int columnCount(const QModelIndex &parent) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;

  bool canFetchMore(const QModelIndex &parent) const override;
  void fetchMore(const QModelIndex &parent) override;

  const BookCard &get(int row) const;

  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role) override;

private slots:
  void onBookDetails(const BookDetails &details);

private:
  QList<BookCard> m_bookCards;

  int m_booksCount;

  struct {
    QPersistentModelIndex index;
    int rating;
  } m_hoverRating;
};

#endif  // BOOKRESTMODEL_H
