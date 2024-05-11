#include <QPaintEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

#include "pagination.h"
#include "ui_pagination.h"

Pagination::Pagination(QWidget *parent) : Pagination(0, 0, parent) {}

Pagination::Pagination(int itemsCount, int itemsPerPageCount, QWidget *parent)
    : QWidget(parent), ui(new Ui::Pagination),
      m_pageValidator(new QIntValidator(0, 0, this)), m_itemsCount(itemsCount),
      m_currentPage(0), m_itemsPerPageCount(itemsPerPageCount) {
  ui->setupUi(this);

  ui->pageInput->setValidator(m_pageValidator);

  updateTotalPages();
  setPage(m_currentPage);

  connect(ui->nextPageButton, &QPushButton::clicked, this,
          &Pagination::nextPage);
  connect(ui->prevPageButton, &QPushButton::clicked, this,
          &Pagination::prevPage);

  connect(ui->pageInput, &QLineEdit::textEdited, this,
          [this](const QString &text) {
            if (ui->pageInput->hasAcceptableInput()) {
              setCurrentPage(text.toInt());
            }
          });
}

void Pagination::nextPage() {
  if (!isNextPageAvailable()) {
    return;
  }

  setCurrentPage(m_currentPage + 1);
}

void Pagination::prevPage() {
  if (!isPrevPageAvailable()) {
    return;
  }

  setCurrentPage(m_currentPage - 1);
}

int Pagination::itemsCount() const {
  return m_itemsCount;
}

void Pagination::setItemsCount(int newItemsCount) {
  m_itemsCount = newItemsCount;
  updateTotalPages();
}

void Pagination::setPage(int newPage) {
  m_currentPage = newPage;
  ui->pageInput->setText(QString::number(m_currentPage));

  updatePageButtons();
}

void Pagination::setCurrentPage(int newPage) {
  if (m_currentPage == newPage) {
    return;
  }
  setPage(newPage);

  emit pageChanged(m_currentPage);
}

int Pagination::itemsPerPageCount() const {
  return m_itemsPerPageCount;
}

void Pagination::setItemsPerPageCount(int newItemsPerPageCount) {
  m_itemsPerPageCount = newItemsPerPageCount;
  updateTotalPages();
}

int Pagination::currentPage() const {
  return m_currentPage;
}

bool Pagination::isPageValid(int page) const {
  return page >= 0 && page < m_totalPages;
}

bool Pagination::isNextPageAvailable() const {
  return isPageValid(m_currentPage + 1);
}

bool Pagination::isPrevPageAvailable() const {
  return isPageValid(m_currentPage - 1);
}

void Pagination::firstPage() {
  setCurrentPage(0);
}

void Pagination::lastPage() {
  setCurrentPage(m_totalPages - 1);
}

int Pagination::calculateTotalPages() const {
  if (!m_itemsCount) {
    return 1;
  }

  return qCeil(float(m_itemsCount) / float(m_itemsPerPageCount));
}

void Pagination::updatePageButtons() {
  ui->prevPageButton->setDisabled(isFirstPage());
  ui->nextPageButton->setDisabled(isLastPage());
}

void Pagination::updateTotalPages() {
  m_totalPages = calculateTotalPages();

  QFontMetrics metrics(ui->pageInput->font());
  QString totalPages = QString::number(m_totalPages - 1);

  ui->pageInput->setMaximumWidth(
    metrics.boundingRect(totalPages + "XXX").width());

  updatePageButtons();

  ui->totalPagesText->setText("of " + totalPages);
  m_pageValidator->setTop(m_totalPages - 1);
}

bool Pagination::isFirstPage() const {
  return m_currentPage == 0;
}

bool Pagination::isLastPage() const {
  return m_currentPage == m_totalPages - 1;
}

void Pagination::resizeEvent(QResizeEvent * /*event*/) {
  /* Align buttons and input */
  int buttonHeight = ui->nextPageButton->height();

  /* BUG: https://bugreports.qt.io/browse/QTBUG-62797 */
  ui->pageInput->setMinimumHeight(buttonHeight - 4);
}
