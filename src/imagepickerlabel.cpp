#include <QCoreApplication>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>

#include "fadebutton.h"
#include "imagepickerlabel.h"

ImagePickerLabel::ImagePickerLabel(QWidget *parent, Qt::AspectRatioMode mode)
    : QLabel(parent), m_imagePath(QString()) {
  m_pickerButton = new FadeButton(false, 500, QEasingCurve::Linear, this);

  setMinimumSize(1, 1);
  setScaledContents(false);

  m_mode = mode;

  QString stylesheet = "image: url(%1)";
  m_pickerButton->setStyleSheet(stylesheet.arg(":/icons/ImagePickerIcon"));

  connect(m_pickerButton, &QPushButton::clicked, this,
          &ImagePickerLabel::handlePickerButtonClick);
}

void ImagePickerLabel::resizeEvent(QResizeEvent *e) {
  int w = width();
  int h = height();

  if (!pixmap().isNull()) {
    QLabel::setPixmap(scaledPixmap());
  }

  int size = qMin(w, h) / 5;
  int margin = 10;

  m_pickerButton->setGeometry(w - size - margin, h - size - margin, size, size);

  QLabel::resizeEvent(e);
}

void ImagePickerLabel::setPixmap(const QPixmap &pixmap) {
  m_pixmap = pixmap;

  QLabel::setPixmap(scaledPixmap());
}

QPixmap ImagePickerLabel::scaledPixmap() const {
  return m_pixmap.scaled(size(), m_mode, Qt::SmoothTransformation);
}

void ImagePickerLabel::enterEvent(QEnterEvent *e) {
  m_pickerButton->fadeIn();

  QLabel::enterEvent(e);
}

void ImagePickerLabel::leaveEvent(QEvent *e) {
  m_pickerButton->fadeOut();

  QLabel::leaveEvent(e);
}

void ImagePickerLabel::handlePickerButtonClick() {
  m_imagePath = QFileDialog::getOpenFileName(
    this, tr("Select Image"), QCoreApplication::applicationDirPath(),
    tr("JPG Files (*.jpg)"));

  if (m_imagePath.isNull()) {
    return;
  }

  QPixmap pixmap(m_imagePath);
  setPixmap(pixmap);
}

void ImagePickerLabel::setAspectRatio(Qt::AspectRatioMode mode) {
  m_mode = mode;

  if (!pixmap().isNull()) {
    QLabel::setPixmap(scaledPixmap());
  }
}

Qt::AspectRatioMode ImagePickerLabel::aspectRatio() {
  return m_mode;
}

QString ImagePickerLabel::imagePath() const {
  return m_imagePath;
}

QSize ImagePickerLabel::sizeHint() const {
  int w = width();
  return QSize(w, heightForWidth(w));
};

int ImagePickerLabel::heightForWidth(int width) const {
  return m_pixmap.isNull() ? height()
                           : m_pixmap.height() * width / m_pixmap.width();
}
