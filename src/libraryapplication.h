#ifndef LIBRARYAPPLICATION_H
#define LIBRARYAPPLICATION_H

#include <QApplication>

class LibraryApplication : public QApplication {
public:
  LibraryApplication(int &argc, char **argv);

private:
  void setupSettings();
};

#endif  // LIBRARYAPPLICATION_H
