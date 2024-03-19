#ifndef LIBRARYAPPLICATION_H
#define LIBRARYAPPLICATION_H

#include <QApplication>

class QSqlError;

class LibraryApplication : public QApplication {
public:
  LibraryApplication(int &argc, char **argv);

private:
  void setupSettings();
};

void databaseErrorMessageBox(QWidget *parent, const QSqlError &e);

#endif  // LIBRARYAPPLICATION_H
