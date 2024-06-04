#include <QMessageBox>
#include <QSettings>
#include <QThread>

#include <QMovie>

#include "libraryapplication.h"
#include "librarymainwindow.h"

LibraryApplication::LibraryApplication(int &argc, char **argv)
    : QApplication(argc, argv) {

  setOrganizationName("LibraryManagementOrganization");
  setOrganizationDomain("LibraryManagementDomain");
  setApplicationName("LibraryManagementSystem");

  /* Format debug messages */
  qSetMessagePattern(
    "[%{time yyyy/MM/dd h:mm:ss.zzz} "
    "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-"
    "critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

  /* INFO: For debug purposes */
  QThread::currentThread()->setObjectName("Main Thread");
  setStyle("windowsvista");

  QFont appFont = font();
  appFont.setPointSize(10);
  setFont(appFont);

  setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}

int LibraryApplication::run() {
  LibraryMainWindow window;
  window.showLoginForm();

  return exec();
}
