#include <QtConcurrent>

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include "book.h"
#include "librarydatabase.h"

QFuture<quint32> BookTable::insert(const Book &book) {
  QString cmd =
    "INSERT INTO book (title, publication_date, copies_owned, cover_path) "
    "VALUES (:title, :publication_date, :copies_owned, :cover_path)";

  SqlBindingHash bindings = {
    {":title", book.title},
    {":publication_date", book.publication_date},
    {":copies_owned", book.copies_owned},
    {":cover_path", book.cover_path},
  };

  return LibraryDatabase::insert(cmd, bindings);
};

QFuture<void> BookTable::remove(quint32 book_id) {
  QString cmd = "DELETE FROM book WHERE book_id = :id";

  SqlBindingHash bindings = {
    {":id", book_id},
  };

  return LibraryDatabase::remove(cmd, bindings);
};

QFuture<void> BookTable::update(const Book &book) {
  QString cmd = "UPDATE book "
                "SET title = :title, publication_date = :publication_date, "
                "copies_owned = :copies_owned, cover_path = :cover_path WHERE "
                "book_id = :book_id";

  SqlBindingHash bindings = {
    {":book_id", book.book_id},
    {":title", book.title},
    {":publication_date", book.publication_date},
    {":copies_owned", book.copies_owned},
    {":cover_path", book.cover_path},
  };

  return LibraryDatabase::update(cmd, bindings);
};
