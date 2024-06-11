#include <QPainter>

#include "elidedlabel.h"
#include "headerlistview.h"

#include <QFrame>
#include <qframe.h>

HeaderListView::HeaderListView(QWidget *parent)
    : QListView(parent), m_elideLabel(new ElidedLabel(this)) {

  m_elideLabel->setBackgroundRole(QPalette::Midlight);
  m_elideLabel->setAutoFillBackground(true);
  m_elideLabel->setAlignment(Qt::AlignCenter);
  m_elideLabel->setElideMode(Qt::ElideRight);

  setHeaderText("Header");
}

QString HeaderListView::headerText() const {
  return m_headerText;
}

void HeaderListView::setHeaderText(const QString &newHeaderText) {
  if (m_headerText == newHeaderText)
    return;

  m_headerText = newHeaderText;
  emit headerTextChanged();

  m_elideLabel->setText(m_headerText);
}

QRect HeaderListView::headerAreaRect() {
  const int margin = 1;

  QRect header = rect();
  header.setHeight(fontMetrics().height() + 2 * margin);

  return header;
}

void HeaderListView::resizeEvent(QResizeEvent *event) {
  QListView::resizeEvent(event);

  QRect headerRect = headerAreaRect();

  QMargins margins = viewportMargins();
  margins.setTop(headerRect.height());
  setViewportMargins(margins);

  m_elideLabel->setGeometry(headerRect);
}
