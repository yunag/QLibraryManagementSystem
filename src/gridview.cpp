#include <QScrollBar>

#include "gridview.h"

GridView::GridView(QWidget *parent)
    : QListView(parent), m_verticalSpacing(4), m_horizontalSpacing(8) {
  setUniformItemSizes(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setViewMode(IconMode);
  setWrapping(true);
}

void GridView::distributeGridSize() {
  /* NOTE: scrollbar->width() will report wrong value  
   * which is necessary for calculating currently available viewport
   * width. See https://stackoverflow.com/questions/16515646/how-to-get-scroll-bar-real-width-in-qt
   */
  int scrollBarWidth = style()->pixelMetric(QStyle::PM_ScrollBarExtent);

  int itemWidth = m_itemSize.width() + m_horizontalSpacing;
  int itemHeight = m_itemSize.height();

  int viewportWidth =
    isVisible() ? viewport()->width() - 2 : width() - scrollBarWidth - 1;

  int numItemsInRow = viewportWidth / itemWidth;
  if (numItemsInRow) {
    int totalItemsWidth = numItemsInRow * itemWidth;

    int remaindedWidth = viewportWidth - totalItemsWidth;
    int evenlyDistributedWidth = qMax(remaindedWidth / numItemsInRow, 0);

    QSize newGridSize(itemWidth + evenlyDistributedWidth,
                      itemHeight + m_verticalSpacing);

    setGridSize(newGridSize);
  }
}

void GridView::resizeEvent(QResizeEvent *event) {
  QListView::resizeEvent(event);
  distributeGridSize();
}

void GridView::setItemSize(QSize itemSize) {
  m_itemSize = itemSize;
}

QSize GridView::itemSize() const {
  return m_itemSize;
}

void GridView::setVerticalSpacing(int spacing) {
  m_verticalSpacing = spacing;
}

int GridView::verticalSpacing() const {
  return m_verticalSpacing;
}

void GridView::setHorizontalSpacing(int spacing) {
  m_horizontalSpacing = spacing;
}

int GridView::horizontalSpacing() const {
  return m_horizontalSpacing;
}
