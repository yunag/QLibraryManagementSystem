#include <QCryptographicHash>
#include <QRandomGenerator>

#include "librarydatabase.h"
#include "user.h"

QFuture<bool> User::validate(const QString &username, const QString &password) {
  QString cmd = "SELECT password, salt FROM user WHERE username = :username";

  SqlBindingHash bindings = {
    {":username", username},
  };

  return LibraryDatabase::exec(cmd, bindings)
    .then([password](const LibraryTable &table) {
      if (!table.size()) {
        return false;
      }

      QString userPassword = table[0][0].toString();
      QString salt = table[0][1].toString();

      QString resultPassword = password + salt;
      QString tryPassword =
        QCryptographicHash::hash(resultPassword.toLocal8Bit(),
                                 QCryptographicHash::Sha256)
          .toHex();

      return userPassword == tryPassword;
    });
}

QByteArray User::generateSalt() {
  static constexpr int kSaltSize = 256;

  QByteArray salt;
  salt.resize(kSaltSize);

  for (int i = 0; i < kSaltSize; ++i) {
    salt[i] = QRandomGenerator::global()->bounded(
      0, std::numeric_limits<quint8>::max());
  }

  return salt;
}
