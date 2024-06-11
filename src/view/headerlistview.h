#ifndef HEADERLISTVIEW_H
#define HEADERLISTVIEW_H

#include <QListView>

class ElidedLabel;

class HeaderListView : public QListView {
  Q_OBJECT

  Q_PROPERTY(QString headerText READ headerText WRITE setHeaderText NOTIFY
               headerTextChanged FINAL)

public:
  HeaderListView(QWidget *parent = nullptr);

  QString headerText() const;
  void setHeaderText(const QString &newHeaderText);

  QRect headerAreaRect();

signals:
  void headerTextChanged();

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  QString m_headerText;
  ElidedLabel *m_elideLabel;
};

#endif /* !HEADERLISTVIEW_H */
