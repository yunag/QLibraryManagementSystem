#ifndef IMAGEPICKERLABEL_H
#define IMAGEPICKERLABEL_H

#include <QLabel>
#include <QObject>

#include "aspectratiolabel.h"

class QGraphicsOpacityEffect;
class QPushButton;
class FadeButton;

class ImagePickerLabel : public AspectRatioLabel {
  Q_OBJECT

public:
  explicit ImagePickerLabel(QWidget *parent = nullptr,
                            Qt::AspectRatioMode mode = Qt::KeepAspectRatio);
  QString imagePath() const;

  void setAlignment(Qt::Alignment alignment);

  void setPixmap(const QPixmap &pixmap) override;

protected:
  void updateButtonGeometry();

  void resizeEvent(QResizeEvent *event) override;
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;

private:
  void handlePickerButtonClick();

private:
  QString m_imagePath;

  QGraphicsOpacityEffect *m_effect;
  FadeButton *m_pickerButton;
};

#endif  // IMAGEPICKERLABEL_H
