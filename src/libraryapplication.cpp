#include <QSettings>
#include <QThread>

#include "libraryapplication.h"

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

  settings.setDefaultFormat(QSettings::IniFormat);
  settings.setPath(QSettings::IniFormat, QSettings::UserScope, configPath);

  if (!settings.contains("DatabaseCredentials")) {
    settings.beginGroup("DatabaseCredentials");

    settings.setValue("dbname", "qlibrarydb");
    settings.setValue("username", "root");
    settings.setValue("password", "");

    settings.endGroup();
  }
}
