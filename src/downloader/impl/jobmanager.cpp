#include "jobmanager.h"
#include "contentjob.h"

JobManager::JobManager(QObject* parent)
  : QObject(parent)
  , m_notifier(this)
{
}

void
JobManager::addJob(QSharedPointer<DownloadJob> job)
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
  m_queue.clear();
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
  if (!m_active.isNull())
    disconnectActive();

  if (m_active->type() == DownloadJob::TafsirFile ||
      m_active->type() == DownloadJob::TranslationFile)
    connect(qobject_cast<ContentJob*>(m_active.data()),
            &ContentJob::fileFound,
            this,
            &JobManager::handleFilesFound);

  connect(
    m_active.data(), &DownloadJob::failed, this, &JobManager::handleFailed);
  connect(m_active.data(),
          &DownloadJob::finished,
          this,
          &JobManager::handleCompleted);
  connect(m_active.data(),
          &DownloadJob::progressed,
          this,
          &JobManager::handleProgressed);
  connect(m_active.data(),
          &DownloadJob::downloadSpeedUpdated,
          this,
          &JobManager::downloadSpeedUpdated);
}

void
JobManager::disconnectActive()
{
  if (m_active->type() == DownloadJob::TafsirFile ||
      m_active->type() == DownloadJob::TranslationFile)
    disconnect(qobject_cast<ContentJob*>(m_active.data()),
               &ContentJob::fileFound,
               this,
               &JobManager::handleFilesFound);

  disconnect(
    m_active.data(), &DownloadJob::failed, this, &JobManager::handleFailed);
  disconnect(m_active.data(),
             &DownloadJob::finished,
             this,
             &JobManager::handleCompleted);
  disconnect(m_active.data(),
             &DownloadJob::progressed,
             this,
             &JobManager::handleProgressed);
  disconnect(m_active.data(),
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
  m_notifier.notifyFailed(m_active.data());
  emit jobFailed(m_active);
}

void
JobManager::handleCompleted()
{
  m_notifier.notifyCompleted(m_active.data());
  emit jobCompleted(m_active);
}

void
JobManager::handleFilesFound()
{
  emit filesFound(m_active);
}

bool
JobManager::isOn() const
{
  return m_isOn;
}

QSharedPointer<DownloadJob>
JobManager::active() const
{
  return m_active;
}

NotificationSender*
JobManager::notifier()
{
  return &m_notifier;
}
