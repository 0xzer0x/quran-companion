#ifndef VERSEFRAME_H
#define VERSEFRAME_H

#include <QFrame>
#include <QVBoxLayout>

class HighlightFrame : public QFrame
{
public:
  HighlightFrame(QWidget* parent = nullptr);
  void highlightFrame();
  void setHighlightColor(int r, int g, int b);

  // QWidget interface
protected:
  void enterEvent(QEnterEvent* event);
  void leaveEvent(QEvent* event);

private:
  int m_colorR = 0;
  int m_colorG = 161;
  int m_colorB = 185;
};

#endif // VERSEFRAME_H
