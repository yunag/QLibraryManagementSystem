#include <QtConcurrent>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QMessageBox>

#include "librarydatabase.h"

LibraryDatabase::LibraryDatabase() {
  m_pool.setMaxThreadCount(1);
  m_pool.setExpiryTimeout(-1);
}

QString LibraryDatabase::threadToConnectionName() {
  return QThread::currentThread()->objectName() + "_connection";
}

template <typename PromiseType>
static void setPromiseSQLException(QPromise<PromiseType> &promise,
                                   const QSqlError &error) {
  promise.setException(std::make_exception_ptr(error));
}

QFuture<void> LibraryDatabase::openImpl(const QString &dbname,
                                        const QString &username,
                                        const QString &password,
                                        const QString &host, int port) {
  return QtConcurrent::run(&m_pool,
                           [=](QPromise<void> &promise) {
                             qDebug()
                               << "Database Open:" << QThread::currentThread();

                             QString conName = threadToConnectionName();

                             QSqlDatabase db =
                               QSqlDatabase::contains(conName)
                                 ? QSqlDatabase::database(conName, false)
                                 : QSqlDatabase::addDatabase("QMYSQL", conName);

                             db.setHostName(host);
                             db.setPort(port);
                             db.setDatabaseName(dbname);
                             db.setUserName(username);
                             db.setPassword(password);

                             if (db.open()) {
                               m_lastSuccessfulConnection = conName;
                             } else {
                               setPromiseSQLException(promise, db.lastError());
                             }
                           })
    .then(this, [this]() {
      QSqlDatabase::cloneDatabase(m_lastSuccessfulConnection,
                                  threadToConnectionName());
    });
}

QFuture<void> LibraryDatabase::reopenImpl() {
  return QtConcurrent::run(&m_pool, [](QPromise<void> &promise) {
    qDebug() << "Database reopen:" << QThread::currentThread();

    QSqlDatabase db = QSqlDatabase::database(threadToConnectionName());
    if (!db.open()) {
      setPromiseSQLException(promise, db.lastError());
    }
  });
}

QFuture<LibraryTable>
LibraryDatabase::execImpl(const QString &cmd, const SqlBindingHash &bindings) {
  return QtConcurrent::run(&m_pool, [=](QPromise<LibraryTable> &promise) {
    qDebug() << "Database exec:" << QThread::currentThread();

    QSqlDatabase db = QSqlDatabase::database(threadToConnectionName());
    LibraryTable result;

    if (!db.isValid() || !db.isOpen()) {
      qWarning() << db.lastError();
      setPromiseSQLException(promise, db.lastError());
      return;
    }

    QSqlQuery query(db);
    query.prepare(cmd);

    for (auto it = bindings.begin(); it != bindings.end(); ++it) {
      query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
      qWarning() << db.lastError();
      qWarning() << query.lastError();

      setPromiseSQLException(promise, query.lastError());
      return;
    }

    result.reserve(query.size());

    int rowIndex = 0;
    while (query.next()) {
      TableRow row;
      row.data.reserve(query.record().count());
      row.index = rowIndex++;

      for (int i = 0; i < query.record().count(); ++i) {
        row.data.push_back(query.value(i));
      }

      result.push_back(std::move(row));
    }

    promise.addResult(std::move(result));
  });
}

QFuture<quint32> LibraryDatabase::insertImpl(const QString &cmd,
                                             const SqlBindingHash &bindings) {

  return QtConcurrent::run(&m_pool, [=](QPromise<quint32> &promise) {
    qDebug() << "Database insert:" << QThread::currentThread();

    QSqlDatabase db = QSqlDatabase::database(threadToConnectionName());

    if (!db.isValid() || !db.isOpen()) {
      qWarning() << db.lastError();
      setPromiseSQLException(promise, db.lastError());
      return;
    }

    QSqlQuery query(db);
    query.prepare(cmd);

    for (auto it = bindings.begin(); it != bindings.end(); ++it) {
      query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
      qWarning() << db.lastError();
      qWarning() << query.lastError();

      setPromiseSQLException(promise, query.lastError());
      return;
    }

    promise.addResult(query.lastInsertId().toUInt());
  });
}

void databaseErrorMessageBox(QWidget *parent, const QSqlError &e) {
  QMessageBox::warning(parent, QObject::tr("Database Fatal Error"),
                       QObject::tr("Database failed with message: ") +
                         e.text());
}
