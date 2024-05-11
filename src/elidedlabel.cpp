#include "elidedlabel.h"

ElidedLabel::ElidedLabel(QWidget *parent)
    : QLabel(parent), m_elideMode(Qt::ElideRight) {}

void ElidedLabel::setText(const QString &text) {
  m_text = text;

  QFontMetrics metrics(font());

  int offset = 5;
  QString elidedText = metrics.elidedText(text, m_elideMode, width() - offset);

  QLabel::setText(elidedText);
}

QString ElidedLabel::text() {
  return m_text;
}

void ElidedLabel::setElideMode(Qt::TextElideMode mode) {
  if (m_elideMode != mode) {
    m_elideMode = mode;

    setText(m_text);
  }
}

Qt::TextElideMode ElidedLabel::elideMode() {
  return m_elideMode;
}
