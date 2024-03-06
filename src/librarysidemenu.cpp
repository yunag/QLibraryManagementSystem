#include "librarysidemenu.h"
#include "forms/ui_librarysidemenu.h"

LibrarySideMenu::LibrarySideMenu(QWidget *parent)
    : QWidget(parent), ui(new Ui::LibrarySideMenu) {
  ui->setupUi(this);

  connect(ui->sideMenuButton, &QPushButton::clicked, this,
          &LibrarySideMenu::sideMenuButtonClicked);
  connect(ui->booksMenu, &ClickableFrame::clicked, this,
          &LibrarySideMenu::booksMenuClicked);

  QPalette p = palette();
  p.setColor(QPalette::Inactive, QPalette::Window, Qt::white);
  p.setColor(QPalette::Active, QPalette::Window, Qt::white);
  setPalette(p);

  toggle();
}

LibrarySideMenu::~LibrarySideMenu() {
  delete ui;
}

void LibrarySideMenu::sideMenuButtonClicked() {
  toggle();
}

void LibrarySideMenu::toggle() {
  ui->authorsText->setHidden(!ui->authorsText->isHidden());
  ui->booksText->setHidden(!ui->booksText->isHidden());
};
