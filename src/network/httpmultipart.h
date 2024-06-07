#ifndef HTTPMULTIPART_H
#define HTTPMULTIPART_H

#include <QHttpMultiPart>

class HttpMultiPart : public QHttpMultiPart {
public:
  using QHttpMultiPart::QHttpMultiPart;

  void addPart(const QString &name, const QByteArray &body);
  void addFilePart(const QString &name, const QString &filePath);
};

#endif /* !HTTPMULTIPART_H */
