#ifndef LIBRARYSIDEMENU_H
#define LIBRARYSIDEMENU_H

#include <QWidget>

namespace Ui {
class LibrarySideMenu;
}

class LibrarySideMenu : public QWidget {
  Q_OBJECT

public:
  explicit LibrarySideMenu(QWidget *parent = nullptr);
  ~LibrarySideMenu();

signals:
  void booksMenuClicked();

private slots:
  void sideMenuButtonClicked();

private:
  Ui::LibrarySideMenu *ui;
};

#endif  // LIBRARYSIDEMENU_H
