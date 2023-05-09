#include "verseframe.h"

HighlightFrame::HighlightFrame(QWidget* parent)
  : QFrame(parent)
{
  this->setLayout(new QVBoxLayout);
}

void
HighlightFrame::highlightFrame()
{
  // rgba(0, 161, 185, 50)
  QString rgba = "rgba(" + QString::number(m_colorR) + ',' +
                 QString::number(m_colorG) + ',' + QString::number(m_colorB) +
                 ",50)";
  setStyleSheet(QString("QFrame#%0 "
                        "{background-color:%1;"
                        "border-radius:4px}")
                  .arg(objectName(), rgba));
}

void
HighlightFrame::setHighlightColor(int r, int g, int b)
{
  m_colorR = r;
  m_colorG = g;
  m_colorB = b;
}

void
HighlightFrame::enterEvent(QEnterEvent* event)
{
  if (styleSheet().isEmpty()) {
    QString rgba = "rgba(" + QString::number(m_colorR) + ',' +
                   QString::number(m_colorG) + ',' + QString::number(m_colorB) +
                   ",20)";
    setStyleSheet(QString("QFrame#%0 "
                          "{background-color:%1;"
                          "border-radius:4px}")
                    .arg(objectName(), rgba));
  }
}

void
HighlightFrame::leaveEvent(QEvent* event)
{
  if (!styleSheet().contains(",50)")) {
    setStyleSheet("");
  }
}
