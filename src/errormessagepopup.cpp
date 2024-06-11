#include <QGraphicsOpacityEffect>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>

#include <QMouseEvent>

#include "errormessagepopup.h"

ErrorMessagePopup::ErrorMessagePopup(QWidget *parent)
    : QWidget(parent), m_opacityEffect(new QGraphicsOpacityEffect(this)) {
  m_animation = new QPropertyAnimation(m_opacityEffect, "opacity", this);
  m_animation->setDuration(200);

  setAutoFillBackground(true);
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setVisible(false);

  m_opacityEffect->setOpacity(0.0);
  setGraphicsEffect(m_opacityEffect);

  m_popupTimer.setSingleShot(true);

  connect(&m_popupTimer, &QTimer::timeout, this,
          &ErrorMessagePopup::hideMessage);
};

void ErrorMessagePopup::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  QRect textRect = rect().adjusted(0, 4, 0, 0);

  QPolygonF polygon;
  polygon << textRect.bottomLeft() << textRect.topLeft()
          << (textRect.topLeft() + QPointF(4, -4))
          << (textRect.topLeft() + QPoint(8, 0)) << textRect.topRight()
          << textRect.bottomRight();

  if (m_side == TopSide) {
    int halfWidth = rect().width() / 2;
    int halfHeight = rect().height() / 2;
    polygon = QTransform()
                .translate(halfWidth, halfHeight)
                .rotate(180, Qt::XAxis)
                .translate(-halfWidth, -halfHeight)
                .map(polygon);

    textRect.moveTopLeft(rect().topLeft());
  }

  QPen pen = painter.pen();
  pen.setJoinStyle(Qt::MiterJoin);
  painter.setPen(pen);

  painter.setBrush(QBrush("#CD084F"));
  painter.drawPolygon(polygon);

  painter.setPen(Qt::white);
  painter.drawText(textRect, Qt::AlignCenter, m_message);
}

QSize ErrorMessagePopup::sizeHint() const {
  QFontMetrics fm(font());
  QSize sufficientSize = fm.boundingRect(m_message).size();
  sufficientSize.setHeight(sufficientSize.height() + 8);
  sufficientSize.setWidth(sufficientSize.width() + 10);

  return sufficientSize;
}

void ErrorMessagePopup::hideMessage() {
  m_animation->setStartValue(m_opacityEffect->opacity());
  m_animation->setEndValue(0.0);

  connect(m_animation, &QPropertyAnimation::finished, this, [this]() {
    if (m_target) {
      m_target->removeEventFilter(this);
    }
    qApp->removeEventFilter(this);

    hide();
  }, Qt::SingleShotConnection);

  m_animation->start();
}

bool ErrorMessagePopup::eventFilter(QObject *obj, QEvent *event) {
  auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);

  if (mouseEvent && m_animation->state() != QAbstractAnimation::Running) {
    QRect globalRect(mapToGlobal(rect().topLeft()), rect().size());

    if (globalRect.contains(mouseEvent->globalPosition().toPoint())) {

      m_opacityEffect->setOpacity(0.5);
    } else {
      m_opacityEffect->setOpacity(1);
    }

    return false;
  }

  if (obj == parentWidget() && m_target) {
    if (event->type() == QEvent::Resize || event->type() == QEvent::Move) {
      QPoint pos = m_target->mapTo(parentWidget(), targetPos());
      move(pos);
    }
  }

  return QWidget::eventFilter(obj, event);
}

void ErrorMessagePopup::showMessage(QWidget *widget, const QString &message,
                                    int duration, Side side) {
  widget->installEventFilter(this);
  m_target = widget;
  m_side = side;

  QPoint pos = widget->mapTo(parentWidget(), targetPos());

  showMessage(pos, message, duration, side);
}

void ErrorMessagePopup::showMessage(const QPoint &pos, const QString &message,
                                    int duration, Side side) {
  m_message = message;
  m_side = side;

  QSize size = sizeHint();
  resize(size);

  if (m_side == BottomSide) {
    move(pos);
  } else {
    move(pos.x(), pos.y() - size.height());
  }

  /* WARNING: This can lead to performance degradation */
  qApp->installEventFilter(this);

  m_animation->setStartValue(0.0);
  m_animation->setEndValue(1.0);
  m_animation->start();

  raise();
  setVisible(true);

  m_popupTimer.setInterval(duration);
  m_popupTimer.start();
}

QPoint ErrorMessagePopup::targetPos() {
  switch (m_side) {
    case TopSide:
      return m_target->rect().topLeft();
    case BottomSide:
      return m_target->rect().bottomLeft();
    default:
      return {};
  }
}
