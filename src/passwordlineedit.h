#ifndef PASSWORDLINEEDIT_H
#define PASSWORDLINEEDIT_H

#include <QLineEdit>

class PasswordLineEdit : public QLineEdit {
  Q_OBJECT

public:
  explicit PasswordLineEdit(QWidget *parent = nullptr);

  void setEchoMode(EchoMode mode);
private slots:
  void eyeTriggered();

private:
  QAction *m_eyeAction;
  QIcon m_eyeClose;
  QIcon m_eyeOpen;
};

#endif  // PASSWORDLINEEDIT_H
