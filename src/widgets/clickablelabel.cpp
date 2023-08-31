/**
 * @file clickablelabel.cpp
 * @brief Implementation file for ClickableLabel
 */

#include "clickablelabel.h"

ClickableLabel::ClickableLabel(QWidget* parent)
  : QLabel(parent)
{
  setCursor(Qt::PointingHandCursor);
}

void
ClickableLabel::mousePressEvent(QMouseEvent* event)
{
  emit clicked();
}
