#ifndef PAGINATION_H
#define PAGINATION_H

#include <QObject>

class Pagination : public QObject {
  Q_OBJECT

public:
  Q_INVOKABLE explicit Pagination(QObject *parent = nullptr);

  Q_PROPERTY(QString currentPageHeader READ currentPageHeader WRITE
               setCurrentPageHeader NOTIFY currentPageHeaderChanged)
  Q_PROPERTY(QString pageCountHeader READ pageCountHeader WRITE
               setPageCountHeader NOTIFY pageCountHeaderChanged)
  Q_PROPERTY(QString totalCountHeader READ totalCountHeader WRITE
               setTotalCountHeader NOTIFY totalCountHeaderChanged)
  Q_PROPERTY(int perPage READ perPage WRITE setPerPage NOTIFY perPageChanged)
  Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY
               currentPageChanged)
  Q_PROPERTY(int pageCount READ pageCount NOTIFY pageCountChanged)

  Q_PROPERTY(int totalCount READ totalCount NOTIFY totalCountChanged)

  int perPage() const;
  int currentPage() const;
  QString currentPageHeader() const;
  int pageCount() const;
  QString pageCountHeader() const;

  int totalCount() const;
  QString totalCountHeader() const;

signals:
  void perPageChanged(int perPage);
  void currentPageChanged(int currentPage);
  void currentPageHeaderChanged(const QString &currentPageHeader);
  void totalCountChanged(int totalCount);
  void totalCountHeaderChanged(const QString &totalCountHeader);
  void pageCountChanged(int pageCount);
  void pageCountHeaderChanged(const QString &pageCountHeader);

public slots:
  void setPerPage(int perPage);
  void setCurrentPage(int currentPage);
  void setCurrentPageHeader(const QString &currentPageHeader);
  void setTotalCount(int totalCount);
  void setTotalCountHeader(const QString &totalCountHeader);

private slots:
  void setPageCount(int pageCount);
  void setPageCountHeader(const QString &pageCountHeader);

private:
  int m_perPage;
  int m_currentPage;
  int m_pageCount;
  int m_totalCount;
  QString m_currentPageHeader;
  QString m_totalCountHeader;
  QString m_pageCountHeader;
};

#endif /* !PAGINATION_H */
