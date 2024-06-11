#ifndef LIBRARYAPPLICATION_H
#define LIBRARYAPPLICATION_H

#include <QApplication>

#include "resourcemanager.h"

#define App (dynamic_cast<LibraryApplication *>(qApp))

class LibraryMainWindow;

class LibraryApplication : public QApplication {
public:
  LibraryApplication(int &argc, char **argv);

  int run();

public:
  ResourceManager *resourceManager() { return &m_resourceManager; }

  LibraryMainWindow *mainWindow() { return m_mainWindow; }

private:
  static void setupSettings();

private:
  ResourceManager m_resourceManager;
  LibraryMainWindow *m_mainWindow;
};

#endif  // LIBRARYAPPLICATION_H
