#ifndef BOOKRESTMODEL_H
#define BOOKRESTMODEL_H

#include <QAbstractListModel>

#include "network/network.h"
#include "schema/schema.h"

#include "bookcard.h"
#include "pagination.h"

class BookRestModel : public QAbstractListModel {
  Q_OBJECT

public:
  explicit BookRestModel(QObject *parent = nullptr);

  enum HorizontalSection {
    Id = 0,
    Title,
    Authors,
    Categories,
    Rating,
    Last = Rating
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

  Q_PROPERTY(QVariantMap filters READ filters WRITE setFilters NOTIFY
               filtersChanged FINAL)
  Q_PROPERTY(Pagination *pagination READ pagination WRITE setPagination NOTIFY
               paginationChanged FINAL)
  Q_PROPERTY(
    QString orderBy READ orderBy WRITE setOrderBy NOTIFY orderByChanged FINAL)

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role) const override;
  QVariant dataForColumn(const QModelIndex &index) const;

  int columnCount(const QModelIndex &parent) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  bool removeRows(int row, int count,
                  const QModelIndex &parent = QModelIndex()) override;

  bool canFetchMore(const QModelIndex &parent) const override;
  void fetchMore(const QModelIndex &parent) override;

  void shouldFetchImages(bool shouldFetchImages);

  const BookCard &get(int row) const;

  void reset();
  void reload();

  QVariant data(const QModelIndex &index, int role) const override;
  bool setData(const QModelIndex &index, const QVariant &value,
               int role) override;

  QVariantMap filters() const;
  void setFilters(const QVariantMap &newFilters);

  Pagination *pagination() const;
  void setPagination(Pagination *newPagination);

  QString orderBy() const;
  void setOrderBy(const QString &newOrderBy);

signals:
  void filtersChanged();
  void paginationChanged();
  void sortChanged();
  void orderByChanged();
  void reloadFinished();

private:
  void updatePagination(ReplyPointer reply);
  void processImageUrl(int row, const QUrl &url);

private slots:
  void onBookData(const BookData &data);

private:
  QList<BookCard> m_bookCards;

  bool m_shouldFetchImages;
  int m_booksCount;

  struct ImageWork {
    ReplyPointer reply;
    QSharedPointer<QMovie> busyIndicator;
  };

  struct {
    QPersistentModelIndex index;
    int rating;
  } m_hoverRating;

  QHash<int, ImageWork> m_imageWork;

  QVariantMap m_filters;
  Pagination *m_pagination = nullptr;
  QString m_orderBy;
};

#endif  // BOOKRESTMODEL_H
