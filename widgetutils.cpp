#include "widgetutils.h"

QString usePlaceholderIfEmpty(QLineEdit *line_edit) {
  QString text = line_edit->text();

  if (text.isEmpty()) {
    text = line_edit->placeholderText();
    line_edit->setText(text);
  }
  return text;
}
