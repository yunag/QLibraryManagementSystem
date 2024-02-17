#ifndef QLIBRARYDATABASE_H
#define QLIBRARYDATABASE_H

#include <QFuture>
#include <QPromise>
#include <QSqlError>
#include <QVariant>

class QSqlDatabase;

using LibraryTable = QList<QVariantList>;
using SqlBindingHash = QHash<QString, QVariant>;

/**
 * @class QLibraryDatabase
 * @brief Async database workaround
 *
 */
class LibraryDatabase : public QObject {
  Q_OBJECT

private:
  template <class Func, class... Args>
  static auto invokeAsFuture(Func func, Args &&...args) {
    LibraryDatabase &db = instance();

    using ReturnType = decltype((db.*func)(args...));

    QSharedPointer<QPromise<ReturnType>> promise(new QPromise<ReturnType>());
    promise->start();

    /* TODO: Perfect forward capture */
    QMetaObject::invokeMethod(
      &db,
      [=, &db]() {
        try {
          if constexpr (!std::is_void<ReturnType>::value) {
            auto res = (db.*func)(args...);
            promise->addResult(std::move(res));
          } else {
            (db.*func)(args...);
          }
        } catch (const QSqlError &err) {
          promise->setException(std::make_exception_ptr(err));
        }
        promise->finish();
      },
      Qt::QueuedConnection);

    return promise->future();
  }

public:
  static LibraryDatabase &instance() {
    static LibraryDatabase instance;
    return instance;
  }

  static QFuture<void> reopen() {
    return invokeAsFuture(&LibraryDatabase::reopenImpl);
  }

  static QFuture<void> transaction() {
    return invokeAsFuture(&LibraryDatabase::transactionImpl);
  }

  static QFuture<void> commit() {
    return invokeAsFuture(&LibraryDatabase::commitImpl);
  }

  static QFuture<void> rollback() {
    return invokeAsFuture(&LibraryDatabase::rollbackImpl);
  }

  static QFuture<void> open(const QString &dbname, const QString &username,
                            const QString &password,
                            const QString &host = "localhost",
                            int port = 3306) {
    return invokeAsFuture(&LibraryDatabase::openImpl, dbname, username,
                          password, host, port);
  }

  static QFuture<LibraryTable> exec(const QString &cmd,
                                    const SqlBindingHash &bindings = {}) {
    return invokeAsFuture(&LibraryDatabase::execImpl, cmd, bindings);
  }

  static QFuture<quint32> insert(const QString &cmd,
                                 const SqlBindingHash &bindings = {}) {
    return invokeAsFuture(&LibraryDatabase::insertImpl, cmd, bindings);
  }

  static QFuture<void> update(const QString &cmd,
                              const SqlBindingHash &bindings = {}) {
    return invokeAsFuture(&LibraryDatabase::execImpl, cmd, bindings)
      .then([](auto) {});
  }

  static QFuture<void> remove(const QString &cmd,
                              const SqlBindingHash &bindings = {}) {
    return invokeAsFuture(&LibraryDatabase::execImpl, cmd, bindings)
      .then([](auto) {});
  }

private:
  LibraryDatabase();

  ~LibraryDatabase();

private slots:
  void reopenImpl();
  void openImpl(const QString &dbname, const QString &username,
                const QString &password, const QString &host, int port);
  LibraryTable execImpl(const QString &cmd, const SqlBindingHash &bindings);
  quint32 insertImpl(const QString &cmd, const SqlBindingHash &bindings);

  void transactionImpl();
  void commitImpl();
  void rollbackImpl();

private:
  /**
   * @brief Database thread
   */
  QThread m_thread;
  QString m_lastSuccessfulConnection;
};

/* TODO: Move function declaration */
void databaseErrorMessageBox(QWidget *parent, const QSqlError &e);

#endif  // QLIBRARYDATABASE_H
