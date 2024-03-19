#ifndef IMAGEPICKERLABEL_H
#define IMAGEPICKERLABEL_H

#include <QLabel>
#include <QObject>

class QGraphicsOpacityEffect;
class QPushButton;
class FadeButton;

class ImagePickerLabel : public QLabel {
  Q_OBJECT

public:
  explicit ImagePickerLabel(QWidget *parent = nullptr,
                            Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);
  QPixmap scaledPixmap() const;
  QString imagePath() const;

  void setAspectRatio(Qt::AspectRatioMode mode);
  Qt::AspectRatioMode aspectRatio();

  void setAlignment(Qt::Alignment alignment);

public slots:
  void setPixmap(const QPixmap &pixmap);

protected:
  void updateButtonGeometry();

  QSize sizeHint() const override;
  int heightForWidth(int width) const override;

  void resizeEvent(QResizeEvent *e) override;
  void enterEvent(QEnterEvent *e) override;
  void leaveEvent(QEvent *e) override;

private:
  void handlePickerButtonClick();

private:
  QPixmap m_pixmap;
  QString m_imagePath;
  Qt::AspectRatioMode m_mode;

  QGraphicsOpacityEffect *m_effect;
  FadeButton *m_pickerButton;
};

#endif  // IMAGEPICKERLABEL_H
