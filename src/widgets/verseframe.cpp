/**
 * @file verseframe.cpp
 * @brief Implementation file for VerseFrame
 */

#include "verseframe.h"

VerseFrame::VerseFrame(QWidget* parent)
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

  m_rgbHover = "rgba(" + QString::number(m_colorR) + ',' +
               QString::number(m_colorG) + ',' + QString::number(m_colorB) +
               ",20)";
  m_rgbSelected = "rgba(" + QString::number(m_colorR) + ',' +
                  QString::number(m_colorG) + ',' + QString::number(m_colorB) +
                  ",60)";
}

void
VerseFrame::setHighlightColor(int r, int g, int b)
{
  m_colorR = r;
  m_colorG = g;
  m_colorB = b;
}

void
VerseFrame::setSelected(bool selected)
{
  if (selected)
    setStyleSheet(m_frameStylesheet.arg(m_rgbSelected));
  else
    setStyleSheet("");

  m_selected = selected;
}

void
VerseFrame::enterEvent(QEnterEvent* event)
{
  if (styleSheet().isEmpty())
    setStyleSheet(m_frameStylesheet.arg(m_rgbHover));
}

void
VerseFrame::leaveEvent(QEvent* event)
{
  if (!m_selected)
    setStyleSheet("");
}
