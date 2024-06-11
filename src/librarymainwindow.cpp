#include <QScreen>
#include <QSettings>
#include <QStateMachine>
#include <QStyleHints>

#include "librarymainwindow.h"
#include "loginform.h"
#include "ui_librarymainwindow.h"

#include "authordetailsdialog.h"
#include "authorsection.h"
#include "bookdetailsdialog.h"
#include "booksection.h"
#include "categorysection.h"
#include "resourcemanager.h"

LibraryMainWindow::LibraryMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::LibraryMainWindow), m_loginForm(nullptr) {
  ui->setupUi(this);

  connect(ui->display, &QStackedWidget::currentChanged, this,
          &LibraryMainWindow::currentWidgetChanged);

  connect(ui->logOutButton, &QPushButton::clicked, this, [this]() {
    hide();

    QSettings settings;
    settings.remove("credentials");

    showLoginForm();
  });

  connect(ui->booksButton, &QPushButton::toggled, this,
          [this]() { ui->display->setCurrentWidget(m_bookSection); });
  connect(ui->authorsButton, &QPushButton::toggled, this,
          [this]() { ui->display->setCurrentWidget(m_authorSection); });
  connect(ui->categoriesButton, &QPushButton::toggled, this,
          [this]() { ui->display->setCurrentWidget(m_categorySection); });
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
    logged();
  } else {
    m_loginForm = new LoginForm;
    m_loginForm->setAttribute(Qt::WA_DeleteOnClose);

    connect(m_loginForm, &LoginForm::logged, this, &LibraryMainWindow::logged);

    m_loginForm->show();
  }
}

void LibraryMainWindow::logged() {
  if (m_loginForm) {
    m_loginForm->close();
  }

  QRect screenRect = QGuiApplication::primaryScreen()->geometry();
  resize(screenRect.width() * 3 / 4, screenRect.height() * 3 / 4);
  show();

  if (m_initialized) {
    return;
  }

  m_initialized = true;

  m_bookSection = new BookSection(this);
  m_authorSection = new AuthorSection(this);
  m_categorySection = new CategorySection(this);
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

  connect(m_authorSection, &AuthorSection::authorsPickingFinished, this,
          [this](const QList<Author> &authors) {
    ui->display->setCurrentWidget(m_bookSection);
    ui->booksButton->toggle();

    ui->booksButton->setEnabled(true);
    ui->categoriesButton->setEnabled(true);
    emit authorsPickerFinished(authors);
  });

  ui->display->addWidget(m_bookSection);
  ui->display->addWidget(m_authorSection);
  ui->display->addWidget(m_categorySection);
}

void LibraryMainWindow::closeEvent(QCloseEvent *event) {
  emit closed();
  QMainWindow::closeEvent(event);
}

void LibraryMainWindow::requestAuthorsPicker() {
  ui->booksButton->setEnabled(false);
  ui->categoriesButton->setEnabled(false);

  m_authorSection->toggleAuthorPickerMode();

  ui->authorsButton->toggle();
}

void LibraryMainWindow::currentWidgetChanged(int index) {
  QWidget *currentWidget = ui->display->widget(index);

  if (currentWidget == m_bookSection) {
    m_bookSection->loadBooks();
  } else if (currentWidget == m_authorSection) {
    m_authorSection->loadAuthors();
  } else if (currentWidget == m_categorySection) {
    m_categorySection->loadCategories();
  }
}

void LibraryMainWindow::requestBookDetails(quint32 id) {
  m_bookDetails->showDetails(id);
  m_bookDetails->activateWindow();
  m_bookDetails->raise();
}

void LibraryMainWindow::requestAuthorDetails(quint32 id) {
  m_authorDetails->showDetails(id);
  m_authorDetails->activateWindow();
  m_authorDetails->raise();
}

void LibraryMainWindow::changeEvent(QEvent *event) {
  QStyleHints *styleHints = QGuiApplication::styleHints();

  switch (event->type()) {
    case QEvent::ThemeChange: {
      if (styleHints->colorScheme() == Qt::ColorScheme::Light) {
        QIcon::setFallbackThemeName("fluent-light");
      } else {
        QIcon::setFallbackThemeName("fluent-dark");
      }
    }
    default:
      break;
  }
}
