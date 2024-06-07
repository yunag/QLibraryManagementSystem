#include <QFile>
#include <QFileInfo>
#include <QMimeDatabase>

#include "httpmultipart.h"

void HttpMultiPart::addPart(const QString &name, const QByteArray &body) {
  QHttpPart httpPart;
  QString dispositionHeader = "form-data; name=\"%1\"";

  httpPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                     dispositionHeader.arg(name));
  httpPart.setBody(body);
  append(httpPart);
}

void HttpMultiPart::addFilePart(const QString &name, const QString &filePath) {
  auto *file = new QFile(filePath);

  QFileInfo fileInfo(filePath);
  QMimeDatabase mimeDatabase;
  QString mimeType = mimeDatabase.mimeTypeForFile(fileInfo).name();

  QHttpPart httpPart;
  httpPart.setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
  httpPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                     QStringLiteral(R"(form-data; name="%1"; filename="%2")")
                       .arg(name)
                       .arg(fileInfo.fileName()));

  file->open(QIODeviceBase::ReadOnly);
  httpPart.setBodyDevice(file);
  file->setParent(this);

  append(httpPart);
}
