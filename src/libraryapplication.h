#ifndef LIBRARYAPPLICATION_H
#define LIBRARYAPPLICATION_H

#include <QApplication>

#include "resourcemanager.h"

#define App (dynamic_cast<LibraryApplication *>(qApp))

class LibraryApplication : public QApplication {
public:
  LibraryApplication(int &argc, char **argv);

  static int run();

public:
  ResourceManager *resourceManager() { return &m_resourceManager; }

private:
  static void setupSettings();

private:
  ResourceManager m_resourceManager;
};

#endif  // LIBRARYAPPLICATION_H
