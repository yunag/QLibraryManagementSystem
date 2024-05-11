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
  resize(screenRect.width() * 3 / 4, screenRect.height() * 3 / 4);
  show();

  m_booksSection = new BookSection(this);
  m_bookDetails = new BookDetailsDialog;
  m_authorDetails = new AuthorDetailsDialog;

  connect(this, &LibraryMainWindow::closed, m_bookDetails, [this] {
    m_bookDetails->deleteLater();
    m_bookDetails->close();
  });
  connect(this, &LibraryMainWindow::closed, m_authorDetails, [this] {
    m_authorDetails->deleteLater();
    m_authorDetails->close();
  });

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
