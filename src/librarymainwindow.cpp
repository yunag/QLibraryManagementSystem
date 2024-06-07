#include <QScreen>
#include <QSettings>

#include "librarymainwindow.h"
#include "loginform.h"
#include "ui_librarymainwindow.h"

#include "authordetailsdialog.h"
#include "authorsection.h"
#include "bookdetailsdialog.h"
#include "booksection.h"
#include "resourcemanager.h"

LibraryMainWindow::LibraryMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::LibraryMainWindow), m_loginForm(nullptr) {
  ui->setupUi(this);

  connect(ui->booksButton, &QPushButton::clicked, this,
          &LibraryMainWindow::booksButtonClicked);
  connect(ui->authorsButton, &QPushButton::clicked, this,
          &LibraryMainWindow::authorsButtonClicked);
}

LibraryMainWindow::~LibraryMainWindow() {
  delete ui;
}

void LibraryMainWindow::showLoginForm() {
  QSettings settings;

  if (settings.contains("credentials/token")) {
    RestApiManager *restManager = ResourceManager::networkManager();
    restManager->setBearerToken(
      settings.value("credentials/token").toByteArray());

    QUrl hostUrl;
    hostUrl.setHost(settings.value("server/host").toString());
    hostUrl.setPort(settings.value("server/port").toInt());
    hostUrl.setScheme("http");

    restManager->setUrl(hostUrl);
    onLogged();
  } else {
    m_loginForm = new LoginForm;
    m_loginForm->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_loginForm, &LoginForm::logged, this,
            &LibraryMainWindow::onLogged);

    m_loginForm->show();
  }
}

void LibraryMainWindow::onLogged() {
  if (m_loginForm) {
    m_loginForm->close();
  }

  QRect screenRect = QGuiApplication::primaryScreen()->geometry();
  resize(screenRect.width() * 3 / 4, screenRect.height() * 3 / 4);
  show();

  m_bookSection = new BookSection(this);
  m_authorSection = new AuthorSection(this);
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
  auto showAuthorDetails = [this](quint32 authorId) {
    m_authorDetails->showDetails(authorId);
    m_authorDetails->activateWindow();
    m_authorDetails->raise();
  };

  connect(m_bookSection, &BookSection::bookDetailsRequested, this,
          showBookDetails);
  connect(m_authorSection, &AuthorSection::authorDetailsRequested, this,
          showAuthorDetails);

  connect(m_authorDetails, &AuthorDetailsDialog::bookDetailsRequested, this,
          showBookDetails);
  connect(m_bookDetails, &BookDetailsDialog::authorDetailsRequested, this,
          showAuthorDetails);

  ui->display->addWidget(m_bookSection);
  ui->display->addWidget(m_authorSection);
}

void LibraryMainWindow::booksButtonClicked() {
  if (ui->display->currentWidget() == m_bookSection) {
    return;
  }

  ui->display->setCurrentWidget(m_bookSection);
  m_bookSection->loadBooks();
}

void LibraryMainWindow::authorsButtonClicked() {
  if (ui->display->currentWidget() == m_authorSection) {
    return;
  }

  ui->display->setCurrentWidget(m_authorSection);
  m_authorSection->loadAuthors();
}

void LibraryMainWindow::closeEvent(QCloseEvent *event) {
  emit closed();
  QMainWindow::closeEvent(event);
}
