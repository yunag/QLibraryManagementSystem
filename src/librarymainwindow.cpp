#include <QScreen>

#include "booksection.h"
#include "librarymainwindow.h"
#include "loginform.h"
#include "ui_librarymainwindow.h"

#include "authordetailsdialog.h"
#include "bookdetailsdialog.h"

LibraryMainWindow::LibraryMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::LibraryMainWindow), m_loginForm(nullptr) {
  ui->setupUi(this);

  connect(ui->booksButton, &QPushButton::clicked, this,
          &LibraryMainWindow::booksButtonClicked);
  connect(ui->sideMenu, &LibrarySideMenu::booksMenuClicked, this,
          &LibraryMainWindow::booksButtonClicked);
}

LibraryMainWindow::~LibraryMainWindow() {
  delete ui;
}

void LibraryMainWindow::showLoginForm() {
  m_loginForm = new LoginForm;
  m_loginForm->setAttribute(Qt::WA_DeleteOnClose);

  connect(m_loginForm, &LoginForm::logged, this, &LibraryMainWindow::onLogged);

  m_loginForm->show();
}

void LibraryMainWindow::onLogged() {
  m_loginForm->close();

  QRect screenRect = QGuiApplication::primaryScreen()->geometry();
  resize(screenRect.width() * 0.75f, screenRect.height() * 0.75f);
  show();

  m_booksSection = new BookSection(this);
  m_bookDetails = new BookDetailsDialog;
  m_authorDetails = new AuthorDetailsDialog;

  connect(this, &LibraryMainWindow::closed, m_bookDetails,
          &QDialog::deleteLater);
  connect(this, &LibraryMainWindow::closed, m_authorDetails,
          &QDialog::deleteLater);

  auto showBookDetails = [this](quint32 bookId) {
    m_bookDetails->showDetails(bookId);
    m_bookDetails->activateWindow();
    m_bookDetails->raise();
  };

  connect(m_booksSection, &BookSection::bookDetailsRequested, this,
          showBookDetails);

  connect(m_authorDetails, &AuthorDetailsDialog::bookDetailsRequested, this,
          showBookDetails);

  connect(m_bookDetails, &BookDetailsDialog::authorDetailsRequested, this,
          [this](quint32 authorId) {
            m_authorDetails->showDetails(authorId);
            m_authorDetails->activateWindow();
            m_authorDetails->raise();
          });

  ui->display->addWidget(m_booksSection);
}

void LibraryMainWindow::booksButtonClicked() {
  if (ui->display->currentWidget() == m_booksSection) {
    return;
  }

  ui->display->setCurrentWidget(m_booksSection);
  m_booksSection->loadBooks();
}

void LibraryMainWindow::closeEvent(QCloseEvent *event) {
  emit closed();
  QMainWindow::closeEvent(event);
}
