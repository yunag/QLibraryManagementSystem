#ifndef LIBRARYDATABASE_H
#define LIBRARYDATABASE_H

#include <QFuture>
#include <QMetaObject>
#include <QMetaProperty>
#include <QPromise>
#include <QSqlQuery>
#include <QVariant>

#include <QRegularExpression>

#include "database/invoker.h"
#include "database/schema.h"

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
  template <typename Func, typename... Args>
  static auto invoke(Func func, Args &&...args) {
    return invokeAsFuture(instance(), func, std::forward<Args>(args)...);
  }

public:
  static LibraryDatabase &instance() {
    static LibraryDatabase instance;
    return instance;
  }

  static QFuture<void> reopen() { return invoke(&LibraryDatabase::reopenImpl); }

  static QFuture<void> transaction() {
    return invoke(&LibraryDatabase::transactionImpl);
  }

  static QFuture<void> commit() { return invoke(&LibraryDatabase::commitImpl); }

  static QFuture<void> rollback() {
    return invoke(&LibraryDatabase::rollbackImpl);
  }

  static QFuture<void> open(const QString &dbname, const QString &username,
                            const QString &password,
                            const QString &host = "localhost",
                            int port = 3306) {
    return invoke(&LibraryDatabase::openImpl, dbname, username, password, host,
                  port);
  }

  static QFuture<LibraryTable> exec(const QString &cmd,
                                    const SqlBindingHash &bindings = {}) {
    return invoke(&LibraryDatabase::execImpl, cmd, bindings);
  }

  template <typename T>
  static QFuture<QList<T>> getEntries() {
    isIheretingSchema<T>();
    return invoke(&LibraryDatabase::getEntriesImpl<T>);
  }

  template <typename T>
  static QFuture<void> deleteById(const QVariant &id) {
    isIheretingSchema<T>();
    return invoke(&LibraryDatabase::deleteByIdImpl<T>, id);
  }

  template <typename T>
  static QFuture<void> updateById(const QVariant &id, const T &newEntry) {
    isIheretingSchema<T>();
    return invoke(&LibraryDatabase::updateByIdImpl<T>, id, newEntry);
  }

  template <typename T>
  static QFuture<QVariant> insert(const T &entry, bool insertKey = false) {
    isIheretingSchema<T>();
    return invoke(&LibraryDatabase::insertImpl<T>, entry, insertKey);
  }

  template <typename T>
  static QFuture<void> insertBatch(const QList<T> &entries,
                                   bool insertKey = false) {
    isIheretingSchema<T>();
    return invoke(&LibraryDatabase::insertBatchImpl<T>, entries, insertKey);
  }

private:
  LibraryDatabase();

  ~LibraryDatabase();

private:
  QSqlDatabase databaseFromThread();

  QSqlQuery prepareQuery(const QString &cmd);
  QSqlQuery execQuery(const QString &cmd);
  QSqlQuery preparedExecQuery(const QString &cmd,
                              const SqlBindingHash &bindings = {});
  QSqlQuery preparedExecBatchQuery(const QString &cmd,
                                   const SqlBindingHash &bindings = {});

  void reopenImpl();
  void openImpl(const QString &dbname, const QString &username,
                const QString &password, const QString &host, int port);
  LibraryTable execImpl(const QString &cmd,
                        const SqlBindingHash &bindings = {});

  void transactionImpl();
  void commitImpl();
  void rollbackImpl();

  template <typename T>
  static inline constexpr void isIheretingSchema() {
    static_assert(
      std::is_base_of<Schema, T>::value,
      "Class not inheriting Schema. Did you forget to inherit Schema?");
  }

private:
  template <typename T>
  QList<T> getEntriesImpl();
  template <typename T>
  void deleteByIdImpl(const QVariant &id);
  template <typename T>
  void updateByIdImpl(const QVariant &id, const T &newEntry);
  template <typename T>
  QVariant insertImpl(const T &entry, bool insertKey = false);
  template <typename T>
  void insertBatchImpl(const QList<T> &entries, bool insertKey = false);

private:
  /**
   * @brief Database thread
   */
  QThread m_thread;
};

#define FOR_EACH_PROPERTY(prop, metaobj)                                       \
  for (int i = metaobj.propertyOffset();                                       \
       i < metaobj.propertyCount() && (prop = metaobj.property(i)).name();     \
       ++i)

template <typename T>
QList<T> LibraryDatabase::getEntriesImpl() {
  QStringList fields;
  QMetaProperty prop;

  FOR_EACH_PROPERTY(prop, T::staticMetaObject) {
    fields << prop.name();
  }

  QString cmd =
    QString("SELECT %1 FROM %2").arg(fields.join(", "), T::schemaName);

  QSqlQuery query = execQuery(cmd);

  QList<T> resutl;
  while (query.next()) {
    T entity;

    FOR_EACH_PROPERTY(prop, T::staticMetaObject) {
      prop.writeOnGadget(&entity, query.value(i));
    }
    resutl.push_back(std::move(entity));
  }

  return resutl;
}

template <typename T>
void LibraryDatabase::deleteByIdImpl(const QVariant &id) {
  QString cmd = "DELETE FROM %1 WHERE %2 = :%2";

  cmd = cmd.arg(T::schemaName, T::primaryKey);

  SqlBindingHash bindings = {{":" + QString(T::primaryKey), id}};
  preparedExecQuery(cmd, bindings);
}

template <typename T>
void LibraryDatabase::updateByIdImpl(const QVariant &id, const T &newEntry) {
  SqlBindingHash bindings;
  QStringList values;
  QMetaProperty prop;

  FOR_EACH_PROPERTY(prop, T::staticMetaObject) {
    if (prop.name() == QString(T::primaryKey)) {
      continue;
    }

    QString assign = "%1 = :%1";
    values << assign.arg(prop.name());

    bindings.insert(QString(":") + prop.name(), prop.readOnGadget(&newEntry));
  }

  QString cmd = QString("UPDATE %1 SET %2 WHERE %3 = :%3")
                  .arg(T::schemaName, values.join(", "), T::primaryKey);

  bindings.insert(QString(":") + T::primaryKey, id);

  preparedExecQuery(cmd, bindings);
}

template <typename T>
QVariant LibraryDatabase::insertImpl(const T &entry, bool insertKey) {
  SqlBindingHash bindings;
  QStringList fields;
  QMetaProperty prop;

  FOR_EACH_PROPERTY(prop, T::staticMetaObject) {
    if (!insertKey && prop.name() == QString(T::primaryKey)) {
      continue;
    }
    fields << prop.name();
    bindings.insert(QString(":") + prop.name(), prop.readOnGadget(&entry));
  }

  QString cmd =
    QString("INSERT INTO %1 (%2) VALUES (%3)")
      .arg(T::schemaName, fields.join(", "))
      .arg(fields.replaceInStrings(QRegularExpression("^"), ":").join(", "));

  QSqlQuery query = preparedExecQuery(cmd, bindings);

  return query.lastInsertId();
}

template <typename T>
void LibraryDatabase::insertBatchImpl(const QList<T> &entries, bool insertKey) {
  Q_ASSERT(!entries.isEmpty());

  QStringList fields;
  SqlBindingHash bindings;
  QMetaProperty prop;

  FOR_EACH_PROPERTY(prop, T::staticMetaObject) {
    if (!insertKey && prop.name() == QString(T::schemaName)) {
      continue;
    }

    fields << prop.name();

    QVariantList values;
    for (auto &entry : entries) {
      values << prop.readOnGadget(&entry);
    }
    bindings.insert(QString(":") + prop.name(), values);
  }

  QString cmd =
    QString("INSERT INTO %1 (%2) VALUES (%3)")
      .arg(T::schemaName, fields.join(", "))
      .arg(fields.replaceInStrings(QRegularExpression("^"), ":").join(", "));

  QSqlQuery query = preparedExecBatchQuery(cmd, bindings);
}

#endif  // LIBRARYDATABASE_H
