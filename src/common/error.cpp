#include <QMessageBox>

#include "common/error.h"

void handleError(QWidget *parent, const std::exception &err) {
  QMessageBox::warning(parent, "Error", err.what());
}

void handleError(QWidget *parent, const NetworkError &err) {
  QMessageBox::warning(parent, "Network Error", err.message());
}
