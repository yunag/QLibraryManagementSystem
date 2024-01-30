#ifndef IMAGEPICKERLABEL_H
#define IMAGEPICKERLABEL_H

#include <QLabel>
#include <QObject>

class QGraphicsOpacityEffect;
class QPushButton;

class ImagePickerLabel : public QLabel {
  Q_OBJECT

public:
  explicit ImagePickerLabel(QWidget *parent = nullptr,
                            Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);
  QPixmap scaledPixmap() const;

  void fade(qreal start, qreal end);
  void setAspectRatio(Qt::AspectRatioMode mode);
  Qt::AspectRatioMode aspectRatio();

public slots:
  void setPixmap(const QPixmap &pixmap);

protected:
  virtual void resizeEvent(QResizeEvent *e) override;
  virtual void enterEvent(QEnterEvent *e) override;
  virtual void leaveEvent(QEvent *e) override;

private:
  void handlePickerButtonClick();

private:
  QPixmap m_pixmap;
  Qt::AspectRatioMode m_mode;

  QGraphicsOpacityEffect *m_effect;
  QPushButton *m_pickerButton;
};

#endif  // IMAGEPICKERLABEL_H
