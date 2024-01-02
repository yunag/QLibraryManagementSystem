#include "librarymainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  /* INFO: For debug purposes */
  QThread::currentThread()->setObjectName("Main Thread");

  /* Application settings */
  a.setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

  LibraryMainWindow window;
  window.showLoginForm();
  return a.exec();
}
