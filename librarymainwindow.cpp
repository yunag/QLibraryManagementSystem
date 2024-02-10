#include <QShowEvent>
#include <qdebug.h>

#include "booksection.h"
#include "librarymainwindow.h"
#include "loginform.h"
#include "ui_librarymainwindow.h"

LibraryMainWindow::LibraryMainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::LibraryMainWindow), m_loginForm(nullptr) {
  ui->setupUi(this);

  m_currentSection = ui->displayWidget;
  m_booksSection = nullptr;

  QString configPath = QApplication::applicationDirPath();
  qApp->setOrganizationName("LibraryManagementOrganization");
  qApp->setOrganizationDomain("LibraryManagementDomain");
  qApp->setApplicationName("LibraryManagementSystem");

  QSettings::setDefaultFormat(QSettings::IniFormat);
  QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, configPath);

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
  resize(screenRect.width() * 0.75f, screenRect.height() * 0.75f);
  show();

  if (!m_booksSection) {
    m_booksSection = new BookSection(ui->centralwidget);
  }
}

void LibraryMainWindow::booksButtonClicked() {
  if (m_currentSection == m_booksSection) {
    return;
  }

  ui->centralwidget->layout()->replaceWidget(m_currentSection, m_booksSection);
  m_currentSection = m_booksSection;

  QCoreApplication::processEvents();

  m_booksSection->loadBooks();
}
