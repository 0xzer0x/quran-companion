#include "contentjob.h"
#include <downloader/tafsirtask.h>
#include <downloader/translationtask.h>

ContentJob::ContentJob(Type type, int idx)
  : m_idx(idx)
  , m_type(type)
  , m_isDownloading(false)
  , m_taskDlr(this)
{
  if (type == DownloadJob::TafsirFile)
    m_task = new TafsirTask(idx);
  else if (type == DownloadJob::TranslationFile)
    m_task = new TranslationTask(idx);

  connect(&m_taskDlr, &TaskDownloader::completed, this, &DownloadJob::finished);
  connect(&m_taskDlr, &TaskDownloader::taskError, this, &DownloadJob::failed);
  connect(
    &m_taskDlr, &TaskDownloader::progressed, this, &DownloadJob::progressed);
  connect(&m_taskDlr,
          &TaskDownloader::downloadSpeedUpdated,
          this,
          &DownloadJob::downloadSpeedUpdated);
}

void
ContentJob::start()
{
  if (m_isDownloading)
    return;
  m_isDownloading = true;
  if (m_task->destination().exists()) {
    emit fileFound();
    return;
  }
  m_taskDlr.process(m_task, &m_netMgr);
}

void
ContentJob::stop()
{
  if (!m_isDownloading)
    return;
  m_taskDlr.cancel();
  m_isDownloading = false;
}

bool
ContentJob::isDownloading()
{
  return m_isDownloading;
}

int
ContentJob::completed()
{
  return m_taskDlr.bytes() / 1024;
}

int
ContentJob::total()
{
  return m_taskDlr.total() / 1024;
}

DownloadJob::Type
ContentJob::type()
{
  return m_type;
}

QString
ContentJob::name()
{
  return m_type == DownloadJob::TafsirFile
           ? m_tafasir.at(m_idx)->displayName()
           : m_translations.at(m_idx)->displayName();
}

ContentJob::~ContentJob()
{
  delete m_task;
}
