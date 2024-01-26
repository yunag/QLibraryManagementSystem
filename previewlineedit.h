#ifndef PREVIEWLINEEDIT_H
#define PREVIEWLINEEDIT_H

#include <QLineEdit>
#include <QObject>

class PreviewLineEdit : public QLineEdit {
  Q_OBJECT

public:
  explicit PreviewLineEdit(QWidget *parent = nullptr,
                           Qt::TextElideMode elideMode = Qt::ElideRight);

  void setText(const QString &text);
  QString text();

  void setElideMode(Qt::TextElideMode mode);
  Qt::TextElideMode elideMode();

  void focusInEvent(QFocusEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;

private:
  QString m_text;

  Qt::TextElideMode m_elideMode;
};

#endif  // PREVIEWLINEEDIT_H
