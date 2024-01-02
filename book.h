#ifndef BOOK_H
#define BOOK_H

#include <QObject>

#include <QSqlDatabase>

class Book : public QObject {
  Q_OBJECT
public:
  explicit Book(QObject *parent = nullptr,
                QString connection = QSqlDatabase::defaultConnection);
  void begin();
  void commit();
  void rollback();

  bool is_open();
  void create();
  void load(quint32 book_id);
  void save();
  void remove();

public:
  quint32 book_id;
  quint32 category_id;
  QString title;
  QString publication_date;
  quint32 copies_owned;

private:
  bool insert();
  bool update();
  bool exec(QSqlQuery &query);

private:
  QString m_connection;
};

#endif // BOOK_H
