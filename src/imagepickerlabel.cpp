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

  QString stylesheet = "QPushButton{ image: url(%1); }";
  m_pickerButton->setStyleSheet(stylesheet.arg(":/icons/ImagePickerIcon"));

  connect(m_pickerButton, &QPushButton::clicked, this,
          &ImagePickerLabel::handlePickerButtonClick);
}

void ImagePickerLabel::updateButtonGeometry() {
  int w = width(), pixmapWidth = w;
  int h = height(), pixmapHeight = h;

  if (!pixmap().isNull()) {
    QLabel::setPixmap(scaledPixmap());

    pixmapWidth = pixmap().width();
    pixmapHeight = pixmap().height();
  }

  int size = qMin(pixmapWidth, pixmapHeight) * 0.25f;
  int margin = 10;

  int xOff = w;
  int yOff = h;
  if (m_mode == Qt::KeepAspectRatio) {
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

void ImagePickerLabel::resizeEvent(QResizeEvent *e) {
  updateButtonGeometry();

  QLabel::resizeEvent(e);
}

void ImagePickerLabel::setPixmap(const QPixmap &pixmap) {
  m_pixmap = pixmap;

  QLabel::setPixmap(scaledPixmap());
  updateButtonGeometry();
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

  updateButtonGeometry();
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

void ImagePickerLabel::setAlignment(Qt::Alignment alignment) {
  QLabel::setAlignment(alignment);

  updateButtonGeometry();
}
