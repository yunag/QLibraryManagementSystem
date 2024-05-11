#ifndef ASPECTRATIOLABEL_H
#define ASPECTRATIOLABEL_H

#include <QLabel>
#include <QPixmap>

class AspectRatioLabel : public QLabel {
public:
  explicit AspectRatioLabel(QWidget *parent = nullptr,
                            Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio);
  QPixmap scaledPixmap() const;

  void setAspectRatio(Qt::AspectRatioMode mode);
  Qt::AspectRatioMode aspectRatio();

  virtual void setPixmap(const QPixmap &pixmap);

protected:
  QSize sizeHint() const override;
  int heightForWidth(int width) const override;

  void resizeEvent(QResizeEvent *event) override;

private:
  QPixmap m_pixmap;
  Qt::AspectRatioMode m_mode;
};

#endif /* !ASPECTRATIOLABEL_H */
