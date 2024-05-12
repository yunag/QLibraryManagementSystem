#include <QMessageBox>
#include <QSettings>
#include <QSqlError>
#include <QThread>

#include <QMovie>

#include "libraryapplication.h"
#include "librarymainwindow.h"

LibraryApplication::LibraryApplication(int &argc, char **argv)
    : QApplication(argc, argv) {
  /* Format debug messages */
  qSetMessagePattern(
    "[%{time yyyy/MM/dd h:mm:ss.zzz} "
    "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-"
    "critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

  /* INFO: For debug purposes */
  QThread::currentThread()->setObjectName("Main Thread");

  setupSettings();

  setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}

void LibraryApplication::setupSettings() {
  QSettings settings;

  /* TODO: Copy config to build dir */
  QString configPath = applicationDirPath() + "/../..";

  setOrganizationName("LibraryManagementOrganization");
  setOrganizationDomain("LibraryManagementDomain");
  setApplicationName("LibraryManagementSystem");

  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, configPath);
}

/* TODO: Move it to resource manager class */
QSharedPointer<QMovie> LibraryApplication::busyIndicator() {
  QSharedPointer<QMovie> movie = m_loadingMovie.lock();
  if (movie) {
    return movie;
  }

  movie = QSharedPointer<QMovie>(new QMovie(":/images/LoadingStarSmall.gif"),
                                 [](QMovie *movie) {
                                   movie->stop();
                                   movie->deleteLater();
                                 });

  m_loadingMovie = movie;
  return movie;
}

int LibraryApplication::run() {
  LibraryMainWindow window;
  window.showLoginForm();

  return exec();
}
