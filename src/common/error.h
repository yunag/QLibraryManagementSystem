#ifndef ERROR_H
#define ERROR_H

#include <exception>

#include "network/networkerror.h"

void handleError(QWidget *parent, const std::exception &err);
void handleError(QWidget *parent, const NetworkError &err);

#endif /* !ERROR_H */
