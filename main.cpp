#include "libraryapplication.h"
#include "librarymainwindow.h"

int main(int argc, char *argv[]) {
  LibraryApplication a(argc, argv);

  LibraryMainWindow window;
  window.showLoginForm();
  return a.exec();
}
