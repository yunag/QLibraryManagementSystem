#ifndef USER_H
#define USER_H

#include <QDate>
#include <QFuture>

class User {

public:
  User(quint32 user_id, QString name, QString username, QString password,
       QString salt, QDate joined_date, bool is_admin = 0, bool active = 1)
      : user_id(user_id), name(std::move(name)), username(std::move(username)),
        password(std::move(password)), salt(std::move(salt)),
        joined_date(joined_date), is_admin(is_admin), active(active) {}

  quint32 user_id;

  QString name;
  QString username;
  QString password;
  QString salt;

  QDate joined_date;

  bool is_admin;
  bool active;

  static QByteArray generateSalt();
  static QFuture<bool> validate(const QString &username,
                                const QString &password);
};

#endif  // USER_H
