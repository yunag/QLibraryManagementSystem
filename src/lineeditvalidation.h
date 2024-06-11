#ifndef LINEEDITVALIDATION_H
#define LINEEDITVALIDATION_H

#include <QWidget>

class QLineEdit;
class ElidedLabel;

class LineEditValidation : public QWidget {
public:
  explicit LineEditValidation(QWidget *parent = nullptr);

private:
  QLineEdit *m_input;
  ElidedLabel *m_errorMessage;
};

#include <QObject>

class ErrorMessagePopup : public QWidget {
public:
  explicit ErrorMessagePopup(QWidget *parent = nullptr);

  void setLineEdit(QLineEdit *lineEdit);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QLineEdit *m_input;
};

#endif /* !LINEEDITVALIDATION_H */
