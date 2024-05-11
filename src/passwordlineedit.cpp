#include "passwordlineedit.h"

PasswordLineEdit::PasswordLineEdit(QWidget *parent)
    : QLineEdit(parent), m_eyeAction(new QAction),
      m_eyeClose(":/icons/eyeClose"), m_eyeOpen(":/icons/eyeOpen") {

  addAction(m_eyeAction, TrailingPosition);

  connect(m_eyeAction, &QAction::triggered, this,
          &PasswordLineEdit::eyeTriggered);
}

void PasswordLineEdit::eyeTriggered() {
  bool isPassword = echoMode() == EchoMode::Password;

  setEchoMode(isPassword ? EchoMode::Normal : EchoMode::Password);
}

void PasswordLineEdit::setEchoMode(EchoMode mode) {
  m_eyeAction->setIcon(mode == EchoMode::Password ? m_eyeClose : m_eyeOpen);

  QLineEdit::setEchoMode(mode);
}
