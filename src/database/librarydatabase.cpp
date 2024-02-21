#include <QtConcurrent>

#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QMessageBox>

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

template <typename PromiseType>
static void setPromiseSQLException(QPromise<PromiseType> &promise,
                                   const QSqlError &error) {
  promise.setException(std::make_exception_ptr(error));
}

void LibraryDatabase::openImpl(const QString &dbname, const QString &dbUsername,
                               const QString &dbPassword, const QString &host,
                               int port) {
  qDebug() << "Database Open:" << QThread::currentThread();

  QString conName = threadToConnectionName();

  QSqlDatabase db = QSqlDatabase::contains(conName)
                      ? QSqlDatabase::database(conName, false)
                      : QSqlDatabase::addDatabase("QMYSQL", conName);

  db.setHostName(host);
  db.setPort(port);
  db.setDatabaseName(dbname);

  if (!db.open(dbUsername, dbPassword)) {
    throw db.lastError();
  }
}

void LibraryDatabase::reopenImpl() {
  qDebug() << "Database reopen:" << QThread::currentThread();

  QSqlDatabase db = databaseFromThread();
  if (!db.open()) {
    throw db.lastError();
  }
}

LibraryTable LibraryDatabase::execImpl(const QString &cmd,
                                       const SqlBindingHash &bindings) {
  qDebug() << "Database exec:" << QThread::currentThread();

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

  if (!query.lastInsertId().isNull()) {
    m_lastInsertId = query.lastInsertId();
  }

  return result;
}

void databaseErrorMessageBox(QWidget *parent, const QSqlError &e) {
  QMessageBox::warning(parent, QObject::tr("Database Fatal Error"),
                       QObject::tr("Database failed with message: ") +
                         e.text());
}

void LibraryDatabase::transactionImpl() {
  qDebug() << "Database transaction:" << QThread::currentThread();

  QSqlDatabase db = databaseFromThread();
  if (!db.transaction()) {
    throw db.lastError();
  }
}

void LibraryDatabase::commitImpl() {
  qDebug() << "Database commit:" << QThread::currentThread();

  QSqlDatabase db = databaseFromThread();
  if (!db.commit()) {
    throw db.lastError();
  }
}

void LibraryDatabase::rollbackImpl() {
  qDebug() << "Database rollback:" << QThread::currentThread();

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
    qWarning() << query.lastError();
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
    qWarning() << query.lastError();
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
    qWarning() << query.lastError();
    throw query.lastError();
  }
  return query;
}
