#include <QSettings>
#include <QStyleHints>
#include <QThread>

#include "libraryapplication.h"
#include "librarymainwindow.h"

LibraryApplication::LibraryApplication(int &argc, char **argv)
    : QApplication(argc, argv) {

  setOrganizationName("LibraryManagementOrganization");
  setOrganizationDomain("LibraryManagementDomain");
  setApplicationName("LibraryManagementSystem");

  if (styleHints()->colorScheme() == Qt::ColorScheme::Light) {
    QIcon::setFallbackThemeName("fluent-light");
  } else {
    QIcon::setFallbackThemeName("fluent-dark");
  }

  /* Format debug messages */
  qSetMessagePattern(
    "[%{time yyyy/MM/dd h:mm:ss.zzz} "
    "%{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-"
    "critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

  /* INFO: For debug purposes */
  QThread::currentThread()->setObjectName("Main Thread");

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
  m_mainWindow = new LibraryMainWindow;
  m_mainWindow->showLoginForm();

  return exec();
}
