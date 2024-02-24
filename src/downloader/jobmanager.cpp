#include "jobmanager.h"
#include "contentjob.h"

JobManager::JobManager(QObject* parent)
  : QObject(parent)
{
}

void
JobManager::addJob(QPointer<DownloadJob> job)
{
  m_queue.enqueue(job);
}

void
JobManager::start()
{
  if (m_isOn)
    return;
  m_isOn = true;
  processJobs();
}

void
JobManager::stop()
{
  if (!m_isOn)
    return;
  m_isOn = false;
  if (!m_active.isNull())
    m_active->stop();
  disconnectActive();
  emit jobAborted();
}

void
JobManager::processJobs()
{
  if (m_queue.isEmpty()) {
    m_isOn = false;
    return;
  }
  m_active = m_queue.dequeue();
  connectActive();
  m_active->start();
}

void
JobManager::connectActive()
{
  if (m_active->type() == DownloadJob::TafsirFile ||
      m_active->type() == DownloadJob::TranslationFile)
    connect(qobject_cast<ContentJob*>(m_active),
            &ContentJob::fileFound,
            this,
            &JobManager::handleFilesFound);

  connect(m_active, &DownloadJob::failed, this, &JobManager::handleFailed);
  connect(m_active, &DownloadJob::finished, this, &JobManager::handleCompleted);
  connect(
    m_active, &DownloadJob::progressed, this, &JobManager::handleProgressed);
  connect(m_active,
          &DownloadJob::downloadSpeedUpdated,
          this,
          &JobManager::downloadSpeedUpdated);
}

void
JobManager::disconnectActive()
{
  if (m_active->type() == DownloadJob::TafsirFile ||
      m_active->type() == DownloadJob::TranslationFile)
    disconnect(qobject_cast<ContentJob*>(m_active),
               &ContentJob::fileFound,
               this,
               &JobManager::handleFilesFound);

  disconnect(m_active, &DownloadJob::failed, this, &JobManager::handleFailed);
  disconnect(
    m_active, &DownloadJob::finished, this, &JobManager::handleCompleted);
  disconnect(
    m_active, &DownloadJob::progressed, this, &JobManager::handleProgressed);
  disconnect(m_active,
             &DownloadJob::downloadSpeedUpdated,
             this,
             &JobManager::downloadSpeedUpdated);
}

void
JobManager::handleProgressed()
{
  emit jobProgressed(m_active);
}

void
JobManager::handleFailed()
{
  emit jobFailed(m_active);
  disconnectActive();
  processJobs();
}

void
JobManager::handleCompleted()
{
  emit jobCompleted(m_active);
  disconnectActive();
  processJobs();
}

void
JobManager::handleFilesFound()
{
  emit filesFound(m_active);
  disconnectActive();
  processJobs();
}

bool
JobManager::isOn() const
{
  return m_isOn;
}

QPointer<DownloadJob>
JobManager::active() const
{
  return m_active;
}

JobManager::~JobManager()
{
  if (!m_queue.isEmpty())
    qDeleteAll(m_queue);
}
