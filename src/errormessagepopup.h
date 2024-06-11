#ifndef LINEEDITVALIDATION_H
#define LINEEDITVALIDATION_H

#include <QTimer>
#include <QWidget>

class QPropertyAnimation;
class QGraphicsOpacityEffect;

class ErrorMessagePopup : public QWidget {
public:
  enum Side { TopSide, BottomSide };

  explicit ErrorMessagePopup(QWidget *parent = nullptr);

  void showMessage(const QPoint &pos, const QString &message,
                   int duration = 3000, Side side = BottomSide);
  void showMessage(QWidget *widget, const QString &message, int duration = 3000,
                   Side side = BottomSide);

  void hideMessage();

protected:
  void paintEvent(QPaintEvent *event) override;

  QSize sizeHint() const override;
  bool eventFilter(QObject *obj, QEvent *event) override;

private:
  QPoint targetPos();

private:
  QPropertyAnimation *m_animation;
  QGraphicsOpacityEffect *m_opacityEffect;

  QTimer m_popupTimer;

  Side m_side;
  QString m_message;
  QWidget *m_target;
};

#endif /* !LINEEDITVALIDATION_H */
