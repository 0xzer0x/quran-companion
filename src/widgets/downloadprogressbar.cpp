/**
 * @file downloadprogressbar.cpp
 * @brief Implementation file for DownloadProgressBar
 */

#include "downloadprogressbar.h"
#include <QApplication>
#include <QPointer>
#include <QStyle>

DownloadProgressBar::DownloadProgressBar(QWidget* parent, Type type, int max)
  : QProgressBar(parent)
  , m_type(type)
{
  setStyling(downloading);
  setMaximum(max);
  setValue(0);
  if (m_type == DownloadJob::TafsirFile ||
      m_type == DownloadJob::TranslationFile)
    setFormat("%v / %m " + qApp->translate("DownloadManager", "KB"));
  else
    setFormat("%v / %m");
}

void
DownloadProgressBar::updateProgress(QSharedPointer<DownloadJob> job)
{
  if (maximum() != job->total())
    setMaximum(job->total());

  setValue(job->completed());
}

void
DownloadProgressBar::finished()
{
  if (m_type == DownloadJob::TafsirFile ||
      m_type == DownloadJob::TranslationFile) {
    setValue(1);
    setMaximum(1);
  }
  setFormat("%v / %m");
}

void
DownloadProgressBar::failed()
{
  if (m_type == DownloadJob::TafsirFile ||
      m_type == DownloadJob::TranslationFile) {
    setValue(0);
    setMaximum(1);
  }
  setFormat("%v / %m");
}

void
DownloadProgressBar::setStyling(State downState)
{
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

  style()->unpolish(this);
  style()->polish(this);
  update();
}
