#include <QSqlQuery>
#include <QSqlRecord>
#include <QtConcurrent>

#include "qlibrarydatabase.h"

QLibraryDatabase::QLibraryDatabase() {
  m_pool.setMaxThreadCount(1);
  m_pool.setExpiryTimeout(-1);

  connect(this, &QLibraryDatabase::connectionCreated, this,
          &QLibraryDatabase::connectionCreatedHandler, Qt::QueuedConnection);
}

QString QLibraryDatabase::threadToConnectionName() {
  return QThread::currentThread()->objectName() + "_connection";
}

void QLibraryDatabase::connectionCreatedHandler() {
  qDebug() << "connectionHandler:" << QThread::currentThread();

  /* Should be created in the Main Thread */
  QSqlDatabase::cloneDatabase(m_lastSuccessfulConnection,
                              threadToConnectionName());
}

QFuture<bool> QLibraryDatabase::open(const QString &dbname,
                                     const QString &username,
                                     const QString &password,
                                     const QString &host, int port) {
  return QtConcurrent::run(&m_pool,
                           [=]() -> bool {
                             qDebug() << "Open:" << QThread::currentThread();

                             QString conName = threadToConnectionName();

                             QSqlDatabase db;
                             if (QSqlDatabase::contains(conName)) {
                               db = QSqlDatabase::database(conName, false);
                             } else {
                               db =
                                   QSqlDatabase::addDatabase("QMYSQL", conName);
                             }

                             db.setHostName(host);
                             db.setPort(port);
                             db.setDatabaseName(dbname);
                             db.setUserName(username);
                             db.setPassword(password);

                             return db.open();
                           })
      .then([this](bool ok) {
        if (ok) {
          m_lastSuccessfulConnection = threadToConnectionName();

          emit connectionCreated();
        }
        return ok;
      });
}

QFuture<bool> QLibraryDatabase::reopen() {
  return QtConcurrent::run(&m_pool, []() -> bool {
    return QSqlDatabase::database(threadToConnectionName()).open();
  });
}

QFuture<QSqlError> QLibraryDatabase::lastError() {
  return QtConcurrent::run(&m_pool, []() -> QSqlError {
    qDebug() << "lastError:" << QThread::currentThread();

    return QSqlDatabase::database(threadToConnectionName()).lastError();
  });
}

QFuture<QLibraryTable> QLibraryDatabase::exec(const QString &cmd) {
  return QtConcurrent::run(&m_pool, [=]() -> QLibraryTable {
    qDebug() << "exec:" << QThread::currentThread();

    QSqlDatabase db = QSqlDatabase::database(threadToConnectionName());
    QLibraryTable result;

    if (!db.isValid() || !db.isOpen()) {
      qWarning() << "Database connection is not valid!";
      return result;
    }

    QSqlQuery query(db);

    if (!query.exec(cmd)) {
      qWarning() << query.lastError();
      qWarning() << db.lastError();
      return result;
    }

    result.reserve(query.size());

    int rowIndex = 0;
    while (query.next()) {
      QTableRow row;
      row.data.reserve(query.record().count());
      row.index = rowIndex++;

      for (int i = 0; i < query.record().count(); ++i) {
        row.data.push_back(query.value(i));
      }

      result.push_back(std::move(row));
    }

    return result;
  });
}
