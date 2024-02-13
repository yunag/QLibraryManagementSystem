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

static QString threadToConnectionName() {

  return "Thread(" +
         QString::number(
           reinterpret_cast<quintptr>(QThread::currentThreadId())) +
         ") connection";
}

static QSqlDatabase databaseFromThread(QString lastSuccessfulConnection) {
  QString threadConnectionName = threadToConnectionName();

  return QSqlDatabase::contains(threadConnectionName)
           ? QSqlDatabase::database(threadConnectionName)
           : QSqlDatabase::cloneDatabase(lastSuccessfulConnection,
                                         threadConnectionName);
}

template <typename PromiseType>
static void setPromiseSQLException(QPromise<PromiseType> &promise,
                                   const QSqlError &error) {
  promise.setException(std::make_exception_ptr(error));
}

QFuture<void> LibraryDatabase::openImpl(const QString &dbname,
                                        const QString &dbUsername,
                                        const QString &dbPassword,
                                        const QString &host, int port) {
  return QtConcurrent::run(&m_pool, [=](QPromise<void> &promise) {
    qDebug() << "Database Open:" << QThread::currentThread();

    QString conName = threadToConnectionName();

    QSqlDatabase db = QSqlDatabase::contains(conName)
                        ? QSqlDatabase::database(conName, false)
                        : QSqlDatabase::addDatabase("QMYSQL", conName);

    db.setHostName(host);
    db.setPort(port);
    db.setDatabaseName(dbname);

    if (db.open(dbUsername, dbPassword)) {
      m_lastSuccessfulConnection = conName;
    } else {
      setPromiseSQLException(promise, db.lastError());
    }
  });
}

QFuture<void> LibraryDatabase::reopenImpl() {
  return QtConcurrent::run(&m_pool, [this](QPromise<void> &promise) {
    qDebug() << "Database reopen:" << QThread::currentThread();

    QSqlDatabase db = databaseFromThread(m_lastSuccessfulConnection);
    if (!db.open()) {
      setPromiseSQLException(promise, db.lastError());
    }
  });
}

QFuture<LibraryTable>
LibraryDatabase::execImpl(const QString &cmd, const SqlBindingHash &bindings) {
  return QtConcurrent::run(&m_pool, [=](QPromise<LibraryTable> &promise) {
    qDebug() << "Database exec:" << QThread::currentThread();

    QSqlDatabase db = databaseFromThread(m_lastSuccessfulConnection);

    if (!db.isValid() || !db.isOpen()) {
      qWarning() << db.lastError();
      setPromiseSQLException(promise, db.lastError());
      return;
    }

    m_lastSuccessfulConnection = threadToConnectionName();

    QSqlQuery query(db);
    query.prepare(cmd);

    for (auto it = bindings.begin(); it != bindings.end(); ++it) {
      query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
      qWarning() << query.lastError();

      setPromiseSQLException(promise, query.lastError());
      return;
    }

    LibraryTable result;
    result.reserve(query.size());

    while (query.next()) {
      QVariantList row;
      row.reserve(query.record().count());

      for (int i = 0; i < query.record().count(); ++i) {
        row.push_back(query.value(i));
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

    QSqlDatabase db = databaseFromThread(m_lastSuccessfulConnection);

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

QFuture<void> LibraryDatabase::transactionImpl() {
  return QtConcurrent::run(&m_pool, [=](QPromise<void> &promise) {
    qDebug() << "Database transaction:" << QThread::currentThread();

    QSqlDatabase db = databaseFromThread(m_lastSuccessfulConnection);
    if (!db.transaction()) {
      setPromiseSQLException(promise, db.lastError());
    }
  });
}

QFuture<void> LibraryDatabase::commitImpl() {
  return QtConcurrent::run(&m_pool, [=](QPromise<void> &promise) {
    qDebug() << "Database commit:" << QThread::currentThread();

    QSqlDatabase db = databaseFromThread(m_lastSuccessfulConnection);
    if (!db.commit()) {
      setPromiseSQLException(promise, db.lastError());
    }
  });
}

QFuture<void> LibraryDatabase::rollbackImpl() {
  return QtConcurrent::run(&m_pool, [=](QPromise<void> &promise) {
    qDebug() << "Database rollback:" << QThread::currentThread();

    QSqlDatabase db = databaseFromThread(m_lastSuccessfulConnection);
    if (!db.rollback()) {
      setPromiseSQLException(promise, db.lastError());
    }
  });
}
