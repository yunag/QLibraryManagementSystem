#ifndef WIDGETUTILS_H
#define WIDGETUTILS_H

#include <QUrl>

#include "network/network.h"

#include "aspectratiolabel.h"

class QLineEdit;

class WidgetUtils {
public:
  static ReplyPointer asyncLoadImage(AspectRatioLabel *label, const QUrl &url);
};

#endif /* !WIDGETUTILS_H */
