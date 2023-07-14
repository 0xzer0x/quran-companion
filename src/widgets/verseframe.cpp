#include "verseframe.h"

HighlightFrame::HighlightFrame(QWidget* parent)
  : QFrame(parent)
{
  this->setLayout(new QVBoxLayout);
  m_frameStylesheet = "QFrame {"
                      "background-color:%1;"
                      "border-radius:4px"
                      "}"
                      "QLabel {"
                      "background-color:transparent"
                      "}";
}

void
HighlightFrame::highlightFrame()
{
  QString rgba = "rgba(" + QString::number(m_colorR) + ',' +
                 QString::number(m_colorG) + ',' + QString::number(m_colorB) +
                 ",60)";

  setStyleSheet(m_frameStylesheet.arg(rgba));
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
    setStyleSheet(m_frameStylesheet.arg(rgba));
  }
}

void
HighlightFrame::leaveEvent(QEvent* event)
{
  if (!styleSheet().contains(",60)")) {
    setStyleSheet("");
  }
}
