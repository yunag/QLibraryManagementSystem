#include <QPaintEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>

#include "paginationwidget.h"
#include "ui_pagination.h"

PaginationWidget::PaginationWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::PaginationWidget),
      m_pageValidator(new QIntValidator(this)) {
  ui->setupUi(this);

  ui->pageInput->setText("1");
  ui->pageInput->setValidator(m_pageValidator);
}

PaginationWidget::~PaginationWidget() {
  delete ui;
}

void PaginationWidget::setPagination(Pagination *pagination) {
  m_pagination = pagination;

  connect(ui->nextPageButton, &QPushButton::clicked, this,
          &PaginationWidget::nextPage);
  connect(ui->prevPageButton, &QPushButton::clicked, this,
          &PaginationWidget::prevPage);
  connect(m_pagination, &Pagination::pageCountChanged, this,
          &PaginationWidget::updateTotalPages);
  connect(m_pagination, &Pagination::currentPageChanged, this,
          &PaginationWidget::currentPageChanged);
  connect(ui->pageInput, &QLineEdit::textEdited, this,
          &PaginationWidget::inputEdited);
}

void PaginationWidget::currentPageChanged(int page) {
  ui->pageInput->setText(QString::number(page + 1));
  updatePageButtons();
}

void PaginationWidget::updatePageButtons() {
  ui->prevPageButton->setDisabled(m_pagination->currentPage() == 0);
  ui->nextPageButton->setDisabled(m_pagination->currentPage() ==
                                  m_pagination->pageCount() - 1);
}

void PaginationWidget::updateTotalPages() {
  QFontMetrics metrics(ui->pageInput->font());
  QString totalPages = QString::number(m_pagination->pageCount());

  ui->pageInput->setMaximumWidth(
    metrics.boundingRect(totalPages + "XXX").width());
  ui->pageInput->setText(QString::number(m_pagination->currentPage() + 1));

  updatePageButtons();

  ui->totalPagesText->setText(tr("of %1").arg(totalPages));
  m_pageValidator->setTop(m_pagination->pageCount() - 1);
}

void PaginationWidget::resizeEvent(QResizeEvent * /*event*/) {
  /* Align buttons and input */
  int buttonHeight = ui->nextPageButton->height();

  /* BUG: https://bugreports.qt.io/browse/QTBUG-62797 */
  ui->pageInput->setMinimumHeight(buttonHeight - 4);
}

void PaginationWidget::nextPage() {
  if (!isNextPageAvailable()) {
    return;
  }

  m_pagination->setCurrentPage(m_pagination->currentPage() + 1);
}

void PaginationWidget::prevPage() {
  if (!isPrevPageAvailable()) {
    return;
  }

  m_pagination->setCurrentPage(m_pagination->currentPage() - 1);
}

bool PaginationWidget::isNextPageAvailable() const {
  return isPageValid(m_pagination->currentPage() + 1);
}

bool PaginationWidget::isPrevPageAvailable() const {
  return isPageValid(m_pagination->currentPage() - 1);
}

bool PaginationWidget::isPageValid(int page) const {
  return page >= 0 && page < m_pagination->pageCount();
}

Pagination *PaginationWidget::pagination() const {
  return m_pagination;
}

void PaginationWidget::inputEdited(const QString &text) {
  if (ui->pageInput->hasAcceptableInput()) {
    m_pagination->setCurrentPage(text.toInt());
  }
}
