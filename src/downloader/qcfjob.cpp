#include "qcfjob.h"
#include <QApplication>

QcfJob::QcfJob()
  : m_completed(0)
  , m_active(0)
  , m_isDownloading(false)
  , m_taskDlr(new TaskDownloader(this))
{
  connect(m_taskDlr, &TaskDownloader::completed, this, &QcfJob::taskFinished);
  connect(m_taskDlr, &TaskDownloader::taskError, this, &QcfJob::taskFailed);
  connect(m_taskDlr,
          &TaskDownloader::downloadSpeedUpdated,
          this,
          &DownloadJob::downloadSpeedUpdated);
}

void
QcfJob::enqueueTasks()
{
  for (int i = 1; i <= 604; i++)
    m_queue.enqueue(QcfTask(i));
}

void
QcfJob::processTasks()
{
  if (m_queue.isEmpty()) {
    m_isDownloading = false;
    return;
  }

  m_active = m_queue.dequeue();
  while (m_active.destination().exists()) {
    m_completed++;
    if (m_completed == 604) {
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
QcfJob::taskFinished()
{
  m_completed++;
  emit DownloadJob::progressed();
  if (m_completed == 604)
    emit DownloadJob::finished();
  processTasks();
}

void
QcfJob::taskFailed()
{
  m_isDownloading = false;
  m_queue.clear();
  emit DownloadJob::failed();
}

void
QcfJob::start()
{
  if (m_isDownloading)
    return;
  if (m_queue.isEmpty())
    enqueueTasks();
  m_isDownloading = true;
  processTasks();
}

void
QcfJob::stop()
{
  if (!m_isDownloading)
    return;
  m_taskDlr->cancel();
  m_isDownloading = false;
  m_queue.clear();
  emit DownloadJob::aborted();
}

bool
QcfJob::isDownloading()
{
  return m_isDownloading;
}

int
QcfJob::completed()
{
  return m_completed;
}

int
QcfJob::total()
{
  return 604;
}

DownloadJob::Type
QcfJob::type()
{
  return DownloadJob::Qcf;
}

QString
QcfJob::name()
{
  return qApp->translate("SettingsDialog", "QCF V2");
}
