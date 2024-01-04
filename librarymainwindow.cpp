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

  connect(ui->booksButton, &QPushButton::clicked, this,
          &LibraryMainWindow::booksButtonClicked);
}

LibraryMainWindow::~LibraryMainWindow() { delete ui; }

void LibraryMainWindow::showLoginForm() {
  m_loginForm = new LoginForm(m_database);
  m_loginForm->setAttribute(Qt::WA_DeleteOnClose);

  connect(m_loginForm, &LoginForm::logged, this, &LibraryMainWindow::onLogged);

  m_loginForm->show();
}

void LibraryMainWindow::onLogged() {
  m_loginForm->close();

  resize(800, 600);
  show();

  if (!m_booksSection) {
    m_booksSection = new BookSection(m_database, ui->centralwidget);
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
