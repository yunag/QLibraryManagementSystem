#ifndef QLIBRARYDATABASE_H
#define QLIBRARYDATABASE_H

#include <QFuture>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QThreadPool>
#include <QVariant>

/**
 * @class QLibraryDatabase
 * @brief Async database workaround
 *
 * @warning: This object should live in the main thread!!!
 *
 */
class QLibraryDatabase : public QObject {
  Q_OBJECT

public:
  struct TableRow {
    quint32 index;
    QList<QVariant> data;
  };

  using Table = QList<TableRow>;

public:
  QLibraryDatabase();
  ~QLibraryDatabase() {}

  QFuture<bool> open(const QString &dbname, const QString &username,
                     const QString &password, const QString &host = "localhost",
                     int port = 3306);
  QFuture<QSqlError> lastError();
  QFuture<Table> exec(const QString &cmd);

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
