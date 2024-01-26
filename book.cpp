#include <QtConcurrent>

#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>

#include "book.h"
#include "qlibrarydatabase.h"

QFuture<int> BookTable::insert(const Book &book) {
  return QtConcurrent::run(
    &LibraryDatabase::instance().m_pool, [book](QPromise<int> &promise) {
      qDebug() << "BookTable insert:" << QThread::currentThread();

      QSqlDatabase db =
        QSqlDatabase::database(LibraryDatabase::threadToConnectionName());

      QSqlQuery query(db);
      QString cmd = "INSERT INTO book (title, publication_date, copies_owned) "
                    "VALUES (:title, :publication_date, :copies_owned)";

      query.prepare(cmd);
      query.bindValue(":title", book.title);
      query.bindValue(":publication_date", book.publication_date);
      query.bindValue(":copies_owned", book.copies_owned);

      if (!query.exec()) {
        qWarning() << query.lastError();
        promise.setException(std::make_exception_ptr(query.lastError()));
      } else {
        promise.addResult(query.lastInsertId().toUInt());
      }
    });
};

QFuture<void> BookTable::remove(const Book &book) {
  return QtConcurrent::run(
    &LibraryDatabase::instance().m_pool, [book](QPromise<void> &promise) {
      qDebug() << "BookTable remove:" << QThread::currentThread();

      QSqlDatabase db =
        QSqlDatabase::database(LibraryDatabase::threadToConnectionName());

      QSqlQuery query(db);
      QString cmd = "DELETE FROM book WHERE book_id = :id";

      query.prepare(cmd);
      query.bindValue(":id", book.book_id);

      if (!query.exec()) {
        qWarning() << query.lastError();
        promise.setException(std::make_exception_ptr(query.lastError()));
      }
    });
};

QFuture<void> BookTable::update(const Book &book) {
  return QtConcurrent::run(
    &LibraryDatabase::instance().m_pool, [book](QPromise<void> &promise) {
      qDebug() << "BookTable update:" << QThread::currentThread();

      QSqlDatabase db =
        QSqlDatabase::database(LibraryDatabase::threadToConnectionName());

      QSqlQuery query(db);
      QString cmd = "UPDATE book "
                    "SET title = :title, publication_date = :publication_date, "
                    "copies_owned = :copies_owned WHERE book_id = :book_id";

      query.prepare(cmd);
      query.bindValue(":book_id", book.book_id);
      query.bindValue(":title", book.title);
      query.bindValue(":publication_date", book.publication_date);
      query.bindValue(":copies_owned", book.copies_owned);

      if (!query.exec()) {
        qWarning() << query.lastError();
        promise.setException(std::make_exception_ptr(query.lastError()));
      }
    });
};
