#ifndef QLIBRARYDATABASE_H
#define QLIBRARYDATABASE_H

#include <QFuture>
#include <QVariant>

#include "book.h"

class QSqlError;

struct TableRow {
  quint32 index;
  QList<QVariant> data;
};

using LibraryTable = QList<TableRow>;
using SqlBindingHash = QHash<QString, QVariant>;

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

  static QFuture<LibraryTable> exec(const QString &cmd,
                                    const SqlBindingHash &bindings = {}) {
    return instance().execImpl(cmd, bindings);
  }

  static QFuture<quint32> insert(const QString &cmd,
                                 const SqlBindingHash &bindings = {}) {
    return instance().insertImpl(cmd, bindings);
  }

  static QFuture<void> update(const QString &cmd,
                              const SqlBindingHash &bindings = {}) {
    return exec(cmd, bindings).then([](LibraryTable table) { Q_UNUSED(table) });
  }

  static QFuture<void> remove(const QString &cmd,
                              const SqlBindingHash &bindings = {}) {
    return exec(cmd, bindings).then([](LibraryTable table) { Q_UNUSED(table) });
  }

private:
  LibraryDatabase();

  ~LibraryDatabase() {}

  QFuture<void> reopenImpl();
  QFuture<void> openImpl(const QString &dbname, const QString &username,
                         const QString &password, const QString &host,
                         int port);
  QFuture<QSqlError> lastErrorImpl();
  QFuture<LibraryTable> execImpl(const QString &cmd,
                                 const SqlBindingHash &bindings);
  QFuture<quint32> insertImpl(const QString &cmd,
                              const SqlBindingHash &bindings);

private:
  static QString threadToConnectionName();

private:
  QThreadPool m_pool;
  QString m_lastSuccessfulConnection;
};

/* TODO: Move function declaration */
void databaseErrorMessageBox(QWidget *parent, const QSqlError &e);

#endif  // QLIBRARYDATABASE_H
