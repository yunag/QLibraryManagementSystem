#ifndef LIBRARYAPPLICATION_H
#define LIBRARYAPPLICATION_H

#include <QApplication>

#include "network/network.h"

#define App (dynamic_cast<LibraryApplication *>(qApp))

class QSqlError;

class LibraryApplication : public QApplication {
public:
  LibraryApplication(int &argc, char **argv);

  static int run();

public:
  RestApiManager *network() { return &m_networkManager; }

  QSharedPointer<QMovie> busyIndicator();

private:
  static void setupSettings();

private:
  RestApiManager m_networkManager;

  QWeakPointer<QMovie> m_loadingMovie;
};

#endif  // LIBRARYAPPLICATION_H
