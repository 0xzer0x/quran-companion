#include "downloadprogressbar.h"

DownloadProgressBar::DownloadProgressBar(QWidget* parent, int max)
  : QProgressBar(parent)
{
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
      ss.append(" QProgressBar::chunk "
                "{border-radius:2px; background-color: #008296;}");
      break;
    case aborted:
      ss.append(" QProgressBar::chunk "
                "{border-radius:2px; background-color: #900D09;}");
      break;
  }

  setStyleSheet(ss);
}
