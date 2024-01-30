#include <QCoreApplication>
#include <QFileDialog>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QResizeEvent>

#include "imagepickerlabel.h"

ImagePickerLabel::ImagePickerLabel(QWidget *parent, Qt::AspectRatioMode mode)
    : QLabel(parent) {
  m_pickerButton = new QPushButton(this);

  setMinimumSize(1, 1);
  setScaledContents(false);

  m_mode = mode;

  m_effect = new QGraphicsOpacityEffect(m_pickerButton);
  m_effect->setOpacity(0);
  m_pickerButton->setGraphicsEffect(m_effect);

  QString stylesheet = "image: url(%1)";
  m_pickerButton->setStyleSheet(
    stylesheet.arg(":/resources/images/ImagePickerIcon"));
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

void ImagePickerLabel::fade(qreal start, qreal end) {
  QPropertyAnimation *animation = new QPropertyAnimation(m_effect, "opacity");

  animation->setDuration(500);
  animation->setStartValue(start);
  animation->setEndValue(end);
  animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void ImagePickerLabel::leaveEvent(QEvent *e) {
  fade(1.0, 0);

  QLabel::leaveEvent(e);
}

void ImagePickerLabel::enterEvent(QEnterEvent *e) {
  fade(0, 1.0);

  QLabel::enterEvent(e);
}

void ImagePickerLabel::handlePickerButtonClick() {
  QString fileName = QFileDialog::getOpenFileName(
    this, tr("Select Image"), QCoreApplication::applicationDirPath(),
    tr("JPG Files (*.jpg)"));

  if (fileName.isNull()) {
    return;
  }

  QPixmap pixmap(fileName);
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
