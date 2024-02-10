#include "widgetutils.h"

QString usePlaceholderIfEmpty(QLineEdit *lineEdit) {
  QString text = lineEdit->text();

  if (text.isEmpty()) {
    text = lineEdit->placeholderText();
    lineEdit->setText(text);
  }
  return text;
}
