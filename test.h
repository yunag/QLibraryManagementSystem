#ifndef QLIBRARYDATABASE_H
#define QLIBRARYDATABASE_H

#include <QFuture>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QThreadPool>
#include <QVariant>

#include "book.h"

class ILibraryDatabase {
public:
  ~ILibraryDatabase() {}

  virtual void fetchUsers() = 0;
  static ILibraryDatabase *create();
};

class LibraryDB : ILibraryDatabase {
  friend class LibraryDatabase;

  void fetchUsers() override {}

private:
  LibraryDB() {}
};

struct TableRow {
  quint32 index;
  QList<QVariant> data;
};

using LibraryTable = QList<TableRow>;

/**
 * @class QLibraryDatabase
 * @brief Async database workaround
 *
 */
class LibraryDatabase : public QObject {
  Q_OBJECT

public:
  friend class BookTable;

  static LibraryDatabase &instance() {
    static LibraryDatabase instance;
    return instance;
  }

  static QFuture<void> reopen() { return instance().reopenImpl(); }

  static QFuture<void> open(const QString &dbname, const QString &username,
                            const QString &password,
                            const QString &host = "localhost",
                            int port = 3306) {
    return instance().openImpl(dbname, username, password, host, port);
  }

  static QFuture<LibraryTable> exec(const QString &cmd) {
    return instance().execImpl(cmd);
  }

  static QSqlError lastError();

  static QSqlDriver *driver();

private:
  LibraryDatabase();

  ~LibraryDatabase() {}

  QFuture<void> reopenImpl();
  QFuture<void> openImpl(const QString &dbname, const QString &username,
                         const QString &password, const QString &host,
                         int port);
  QFuture<QSqlError> lastErrorImpl();
  QFuture<LibraryTable> execImpl(const QString &cmd);

private:
  static QString threadToConnectionName();

private:
  QThreadPool m_pool;
  QString m_lastSuccessfulConnection;
};

#endif  // QLIBRARYDATABASE_H
