#include <QSettings>
#include <QThread>

#include "libraryapplication.h"
#include "librarymainwindow.h"

#include <chrono>
using namespace std::chrono_literals;

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

  setupSettings();

  setHighDpiScaleFactorRoundingPolicy(
    Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}

void LibraryApplication::setupSettings() {
  QSettings settings;

  auto ensureSettings = [&](QAnyStringView key, const QVariant &value) {
    if (!settings.contains(key)) {
      settings.setValue(key, value);
    }
  };

  settings.beginGroup("general");
  /* Average typing speed: 200cpm */
  ensureSettings("msSearchTimerUpdateSpeed", 300);
  settings.endGroup();
}

int LibraryApplication::run() {
  LibraryMainWindow window;
  window.showLoginForm();

  return exec();
}
