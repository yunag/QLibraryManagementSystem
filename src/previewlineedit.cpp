#include <QInputMethodEvent>

#include "previewlineedit.h"

PreviewLineEdit::PreviewLineEdit(QWidget *parent, Qt::TextElideMode elideMode)
    : QLineEdit(parent), m_elideMode(elideMode) {

  connect(this, &QLineEdit::textEdited, this,
          [this](const QString &text) { m_text = text; });
}

void PreviewLineEdit::setText(const QString &text) {
  m_text = text;

  QFontMetrics metrics(font());

  int offset = 5;
  QString elidedText = metrics.elidedText(text, m_elideMode, width() - offset);

  QLineEdit::setText(elidedText);
}

void PreviewLineEdit::focusInEvent(QFocusEvent *event) {
  QLineEdit::setText(m_text);

  QLineEdit::focusInEvent(event);
}

void PreviewLineEdit::focusOutEvent(QFocusEvent *event) {
  setText(m_text);

  QLineEdit::focusOutEvent(event);
}

QString PreviewLineEdit::text() {
  return m_text;
}

void PreviewLineEdit::setElideMode(Qt::TextElideMode mode) {
  if (m_elideMode != mode) {
    m_elideMode = mode;

    setText(m_text);
  }
}

Qt::TextElideMode PreviewLineEdit::elideMode() {
  return m_elideMode;
}
