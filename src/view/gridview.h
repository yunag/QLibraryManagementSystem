#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include <QListView>

class GridView : public QListView {
public:
  explicit GridView(QWidget *parent = nullptr);

  void setItemSize(QSize itemSize);
  QSize itemSize() const;

  void setVerticalSpacing(int spacing);
  int verticalSpacing() const;

  void setHorizontalSpacing(int spacing);
  int horizontalSpacing() const;

  void distributeGridSize();

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  QSize m_itemSize;

  int m_verticalSpacing;
  int m_horizontalSpacing;
};

#endif /* !GRIDVIEW_H */
