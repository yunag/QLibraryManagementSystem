#include <QCoreApplication>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>

#include "fadebutton.h"
#include "imagepickerlabel.h"

ImagePickerLabel::ImagePickerLabel(QWidget *parent, Qt::AspectRatioMode mode)
    : AspectRatioLabel(parent, mode) {
  m_pickerButton = new FadeButton(false, 500, QEasingCurve::Linear, this);

  QString stylesheet = "QPushButton{ image: url(%1); }";
  m_pickerButton->setStyleSheet(stylesheet.arg(":/icons/ImagePickerIcon"));

  connect(m_pickerButton, &QPushButton::clicked, this,
          &ImagePickerLabel::handlePickerButtonClick);
}

void ImagePickerLabel::updateButtonGeometry() {
  int w = width();
  int pixmapWidth = w;

  int h = height();
  int pixmapHeight = h;

  if (!pixmap().isNull()) {
    pixmapWidth = pixmap().width();
    pixmapHeight = pixmap().height();
  }

  int size = qMin(pixmapWidth, pixmapHeight) / 4;
  int margin = 10;

  int xOff = w;
  int yOff = h;
  if (aspectRatio() == Qt::KeepAspectRatio) {
    if (alignment() & Qt::AlignLeft) {
      xOff = pixmapWidth;
    }
    if (alignment() & Qt::AlignTop) {
      yOff = pixmapHeight;
    }
    if (alignment() & Qt::AlignHCenter) {
      xOff = (pixmapWidth + w) / 2;
    }
    if (alignment() & Qt::AlignVCenter) {
      yOff = (pixmapHeight + h) / 2;
    }
  }

  m_pickerButton->setGeometry(xOff - size - margin, yOff - size - margin, size,
                              size);
}

void ImagePickerLabel::resizeEvent(QResizeEvent *event) {
  AspectRatioLabel::resizeEvent(event);

  updateButtonGeometry();
}

void ImagePickerLabel::setPixmap(const QPixmap &pixmap) {
  AspectRatioLabel::setPixmap(pixmap);

  updateButtonGeometry();
}

void ImagePickerLabel::enterEvent(QEnterEvent *event) {
  m_pickerButton->fadeIn();

  AspectRatioLabel::enterEvent(event);
}

void ImagePickerLabel::leaveEvent(QEvent *event) {
  m_pickerButton->fadeOut();

  AspectRatioLabel::leaveEvent(event);
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

QString ImagePickerLabel::imagePath() const {
  return m_imagePath;
}

void ImagePickerLabel::setAlignment(Qt::Alignment alignment) {
  AspectRatioLabel::setAlignment(alignment);

  updateButtonGeometry();
}
