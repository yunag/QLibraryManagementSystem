#include <QtConcurrent>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "database/librarydatabase.h"

LibraryDatabase::LibraryDatabase() {
  moveToThread(&m_thread);

  m_thread.setObjectName("Database thread");
  m_thread.start();
}

LibraryDatabase::~LibraryDatabase() {
  m_thread.quit();
  m_thread.wait();
}

static QString threadToConnectionName() {

  return "Thread(" +
         QString::number(
           reinterpret_cast<quintptr>(QThread::currentThreadId())) +
         ") connection";
}

QSqlDatabase LibraryDatabase::databaseFromThread() {
  QString threadConnectionName = threadToConnectionName();

  return QSqlDatabase::database(threadConnectionName);
}

void LibraryDatabase::openImpl(const QString &dbname, const QString &dbUsername,
                               const QString &dbPassword, const QString &host,
                               int port) {
  QString conName = threadToConnectionName();

  QSqlDatabase db = QSqlDatabase::contains(conName)
                      ? QSqlDatabase::database(conName, false)
                      : QSqlDatabase::addDatabase("QMARIADB", conName);

  db.setHostName(host);
  db.setPort(port);
  db.setDatabaseName(dbname);

  if (!db.open(dbUsername, dbPassword)) {
    throw db.lastError();
  }
}

void LibraryDatabase::reopenImpl() {
  QSqlDatabase db = databaseFromThread();
  if (!db.open()) {
    throw db.lastError();
  }
}

LibraryTable LibraryDatabase::execImpl(const QString &cmd,
                                       const SqlBindingHash &bindings) {
  QSqlQuery query = preparedExecQuery(cmd, bindings);

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

  return result;
}

void LibraryDatabase::transactionImpl() {
  QSqlDatabase db = databaseFromThread();
  if (!db.transaction()) {
    throw db.lastError();
  }
}

void LibraryDatabase::commitImpl() {
  QSqlDatabase db = databaseFromThread();
  if (!db.commit()) {
    throw db.lastError();
  }
}

void LibraryDatabase::rollbackImpl() {
  QSqlDatabase db = databaseFromThread();
  if (!db.rollback()) {
    throw db.lastError();
  }
}

QSqlQuery LibraryDatabase::execQuery(const QString &cmd) {
  QSqlDatabase db = databaseFromThread();

  if (!db.isValid() || !db.isOpen()) {
    qWarning() << db.lastError();
    throw db.lastError();
  }

  QSqlQuery query(db);

  if (!query.exec(cmd)) {
    qWarning() << query.lastError() << cmd;
    throw query.lastError();
  }

  return query;
}

QSqlQuery LibraryDatabase::prepareQuery(const QString &cmd) {
  QSqlDatabase db = databaseFromThread();

  if (!db.isValid() || !db.isOpen()) {
    qWarning() << db.lastError();
    throw db.lastError();
  }

  QSqlQuery query(db);
  query.prepare(cmd);

  return query;
}

QSqlQuery LibraryDatabase::preparedExecQuery(const QString &cmd,
                                             const SqlBindingHash &bindings) {

  QSqlQuery query = prepareQuery(cmd);

  for (auto it = bindings.begin(); it != bindings.end(); ++it) {
    query.bindValue(it.key(), it.value());
  }

  if (!query.exec()) {
    qWarning() << query.lastError() << cmd;
    throw query.lastError();
  }

  return query;
}

QSqlQuery
LibraryDatabase::preparedExecBatchQuery(const QString &cmd,
                                        const SqlBindingHash &bindings) {
  QSqlQuery query = prepareQuery(cmd);

  for (auto it = bindings.begin(); it != bindings.end(); ++it) {
    query.bindValue(it.key(), it.value());
  }

  if (!query.execBatch()) {
    qWarning() << query.lastError() << cmd;
    throw query.lastError();
  }
  return query;
}
