#ifndef QLIBRARYDATABASE_H
#define QLIBRARYDATABASE_H

#include <QFuture>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QThreadPool>
#include <QVariant>

struct QTableRow {
  quint32 index;
  QList<QVariant> data;
};

using QLibraryTable = QList<QTableRow>;

/**
 * @class QLibraryDatabase
 * @brief Async database workaround
 *
 */
class QLibraryDatabase : public QObject {
  Q_OBJECT

public:
  QLibraryDatabase();
  ~QLibraryDatabase() {}

  QFuture<bool> reopen();
  QFuture<bool> open(const QString &dbname, const QString &username,
                     const QString &password, const QString &host = "localhost",
                     int port = 3306);
  QFuture<QSqlError> lastError();
  QFuture<QLibraryTable> exec(const QString &cmd);

private:
  static QString threadToConnectionName();

public slots:
  void connectionCreatedHandler();

signals:
  void connectionCreated();

private:
  QString m_lastSuccessfulConnection;
  QThreadPool m_pool;
};

#endif // QLIBRARYDATABASE_H
