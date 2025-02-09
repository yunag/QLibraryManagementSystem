#include "librarysidemenu.h"
#include "ui_librarysidemenu.h"

LibrarySideMenu::LibrarySideMenu(QWidget *parent)
    : QWidget(parent), ui(new Ui::LibrarySideMenu) {
  ui->setupUi(this);

  connect(ui->sideMenuButton, &QPushButton::clicked, this,
          &LibrarySideMenu::sideMenuButtonClicked);
  connect(ui->booksMenu, &ClickableFrame::clicked, this,
          &LibrarySideMenu::booksMenuClicked);

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
