#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QLabel>

class ElidedLabel : public QLabel {
  Q_OBJECT

public:
  explicit ElidedLabel(QWidget *parent = nullptr);

  void setText(const QString &text);
  QString text();

  void setElideMode(Qt::TextElideMode mode);
  Qt::TextElideMode elideMode();

private:
  QString m_text;
  Qt::TextElideMode m_elideMode;
};

#endif  // ELIDEDLABEL_H
