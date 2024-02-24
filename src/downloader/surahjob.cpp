#include "surahjob.h"

SurahJob::SurahJob(int reciter, int surah)
  : m_reciter(reciter)
  , m_surah(surah)
  , m_surahCount(m_dbMgr->getSurahVerseCount(surah))
  , m_taskDlr(new TaskDownloader(this))
{
  connect(m_taskDlr, &TaskDownloader::completed, this, &SurahJob::taskFinished);
  connect(m_taskDlr, &TaskDownloader::taskError, this, &SurahJob::taskFailed);
  connect(m_taskDlr,
          &TaskDownloader::downloadSpeedUpdated,
          this,
          &DownloadJob::downloadSpeedUpdated);
}

void
SurahJob::enqueueTasks()
{
  for (int i = 1; i <= m_surahCount; i++)
    m_queue.enqueue(RecitationTask(m_reciter, m_surah, i));
}

void
SurahJob::processTasks()
{
  if (m_queue.isEmpty()) {
    m_isDownloading = false;
    return;
  }

  m_active = m_queue.dequeue();
  while (m_active.destination().exists()) {
    m_completed++;
    if (m_completed == m_surahCount) {
      emit DownloadJob::progressed();
      emit DownloadJob::finished();
    }

    if (m_queue.isEmpty())
      return;

    m_active = m_queue.dequeue();
  }

  m_taskDlr->process(&m_active, &m_netMgr);
}

void
SurahJob::taskFinished()
{
  m_completed++;
  emit DownloadJob::progressed();
  if (m_completed == m_surahCount)
    emit DownloadJob::finished();
  processTasks();
}

void
SurahJob::start()
{
  if (m_isDownloading)
    return;
  if (m_queue.isEmpty())
    enqueueTasks();
  m_isDownloading = true;
  processTasks();
}

void
SurahJob::stop()
{
  if (!m_isDownloading)
    return;
  m_taskDlr->cancel();
  m_isDownloading = false;
  m_queue.clear();
  emit DownloadJob::aborted();
}

void
SurahJob::taskFailed()
{
  m_isDownloading = false;
  m_queue.clear();
  emit DownloadJob::failed();
}

bool
SurahJob::isDownloading()
{
  return m_isDownloading;
}

int
SurahJob::completed()
{
  return m_completed;
}

int
SurahJob::total()
{
  return m_surahCount;
}

DownloadJob::Type
SurahJob::type()
{
  return DownloadJob::Recitation;
}

QString
SurahJob::name()
{
  return m_reciters.at(m_reciter)->displayName() + " - " +
         m_dbMgr->surahNameList().at(m_surah - 1);
}

int
SurahJob::reciter() const
{
  return m_reciter;
}

int
SurahJob::surah() const
{
  return m_surah;
}

SurahJob::~SurahJob() {}
