#include "downloadprogressbar.h"

DownloadProgressBar::DownloadProgressBar(QWidget* parent, int max)
  : QProgressBar(parent)
{
  qInfo() << this->styleSheet();
  setStyling(downloading);
  setFormat("%v / %m");
  setMaximum(max);
  setValue(0);
}

void
DownloadProgressBar::updateProgress(qint64 downloaded, qint64 total)
{
  if (maximum() != total)
    setMaximum(total);

  setValue(downloaded);
}

void
DownloadProgressBar::setStyling(State downState)
{
  QString ss = m_defStylesheet;
  switch (downState) {
    case downloading:
      break;
    case completed:
      ss.append(
        " QProgressBar::chunk "
        "{border-radius:2px; background-color: qlineargradient(x1:0, y1:0, "
        "x2:0, y2:1, stop:0.5 #00a57f, stop:1 #00916f);}");
      break;
    case aborted:
      ss.append(
        " QProgressBar::chunk "
        "{border-radius:2px; background-color: qlineargradient(x1:0, y1:0, "
        "x2:0, y2:1, stop:0.5 #a50500, stop:1 #930400);}");
      break;
  }

  setStyleSheet(ss);
}
