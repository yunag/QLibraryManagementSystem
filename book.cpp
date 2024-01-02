#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "book.h"

Book::Book(QObject *parent, QString connection)
    : QObject{parent}, m_connection(std::move(connection)) {
  create();
}

void Book::begin() { QSqlDatabase::database(m_connection).transaction(); }

void Book::commit() { QSqlDatabase::database(m_connection).commit(); }

void Book::rollback() { QSqlDatabase::database(m_connection).rollback(); }

bool Book::is_open() { return QSqlDatabase::database(m_connection).isOpen(); }

void Book::create() { book_id = 0; }

void Book::load(quint32 book_id_) {
  QSqlQuery query;
  QString cmd = "SELECT book_id, category_id, title, publication_date, "
                "copies_owned FROM book WHERE book_id = :id";
  query.prepare(cmd);
  query.bindValue(":id", book_id_);

  if (!exec(query)) {
    return;
  }

  while (query.next()) {
    QSqlRecord record = query.record();

    book_id = record.value(0).toUInt();
    category_id = record.value(1).toUInt();
    title = record.value(2).toString();
    publication_date = record.value(3).toString();
    copies_owned = record.value(4).toUInt();
  }
}

void Book::save() {
  if (book_id > 0) {
    update();
  } else {
    insert();
  }
}

void Book::remove() {
  QSqlQuery query;
  QString cmd = "DELETE FROM book WHERE book_id = :id";

  query.prepare(cmd);
  query.bindValue(":id", book_id);

  exec(query);
}

bool Book::insert() {
  QSqlQuery query;
  QString cmd =
      "INSERT INTO book (category_id, title, publication_date, copies_owned) "
      "VALUES (:category_id, :title, :publication_date, :copies_owned)";

  query.prepare(cmd);
}

bool Book::update() {}

bool Book::exec(QSqlQuery &query) {
  QSqlDatabase db = QSqlDatabase::database(m_connection);
  if (!db.isOpen()) {
    qInfo() << "Database connection failure:" << db.lastError();
    return false;
  }

  if (!query.exec()) {
    qWarning() << "Query execution failure:" << query.lastError();
    return false;
  }

  return true;
}
