#include "translationjob.h"

TranslationJob::TranslationJob(int idx)
  : m_idx(idx)
  , m_task(TranslationTask(idx))
  , m_taskDlr(new TaskDownloader(this))
{
  connect(m_taskDlr, &TaskDownloader::completed, this, &DownloadJob::finished);
  connect(m_taskDlr, &TaskDownloader::taskError, this, &DownloadJob::failed);
  connect(m_taskDlr,
          &TaskDownloader::downloadSpeedUpdated,
          this,
          &DownloadJob::downloadSpeedUpdated);
}

void
TranslationJob::start()
{
  if (m_isDownloading)
    return;
  m_isDownloading = true;
  m_taskDlr->process(&m_task, &m_netMgr);
}

void
TranslationJob::stop()
{
  if (!m_isDownloading)
    return;
  m_taskDlr->cancel();
  m_isDownloading = false;
}

bool
TranslationJob::isDownloading()
{
}

int
TranslationJob::completed()
{
}

int
TranslationJob::total()
{
}

DownloadJob::Type
TranslationJob::type()
{
}

QString
TranslationJob::name()
{
}

TranslationJob::~TranslationJob() {}
