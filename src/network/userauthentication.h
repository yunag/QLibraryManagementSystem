#ifndef USERAUTHENTICATION_H
#define USERAUTHENTICATION_H

#include <QFuture>
#include <QUrl>

class RestApiManager;

class UserAuthentication {
public:
  UserAuthentication(RestApiManager *networkManager)
      : m_manager(networkManager) {}

  QFuture<QByteArray> login(const QUrl &host, const QString &username,
                            const QString &password);
  QFuture<void> registerUser(const QUrl &host, const QString &username,
                             const QString &password);

private:
  RestApiManager *m_manager;
};

#endif /* !USERAUTHENTICATION_H */
