/**
 * @file downloadprogressbar.cpp
 * @brief Implementation file for DownloadProgressBar
 */

#include "downloadprogressbar.h"

DownloadProgressBar::DownloadProgressBar(QWidget* parent,
                                         DownloadType type,
                                         int max)
  : QProgressBar(parent)
{
  setStyling(downloading);
  setMaximum(max);
  setValue(0);
  if (type == File)
    setFormat("%v / %m " + qApp->translate("DownloadManager", "KB"));
  else
    setFormat("%v / %m");
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
  // QString ss = m_defStylesheet;
  switch (downState) {
    case downloading:
      break;
    case completed:
      setProperty("progress-state", 1);
      break;
    case aborted:
      setProperty("progress-state", 2);
      break;
  }
}
