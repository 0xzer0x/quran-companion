#ifndef JOBMANAGER_H
#define JOBMANAGER_H

#include <QObject>
#include <QQueue>
#include <QSharedPointer>
#include <downloader/downloadjob.h>
#include <notifiers/jobnotifier.h>

class JobManager : public QObject
{
  Q_OBJECT
public:
  explicit JobManager(QObject* parent);

  void start();
  void stop();
  void addJob(QSharedPointer<DownloadJob> job);

  bool isOn() const;
  QSharedPointer<DownloadJob> active() const;
  const JobNotifier* notifier() const;

public slots:
  void processJobs();

signals:
  void jobAborted();
  void jobCompleted(QSharedPointer<DownloadJob> job);
  void jobFailed(QSharedPointer<DownloadJob> job);
  void jobProgressed(QSharedPointer<DownloadJob> job);
  void downloadSpeedUpdated(int speed, QString unit);
  void filesFound(QSharedPointer<DownloadJob> job);

private slots:
  void handleProgressed();
  void handleFailed();
  void handleCompleted();
  void handleFilesFound();

private:
  void connectActive();
  void disconnectActive();
  QQueue<QSharedPointer<DownloadJob>> m_queue;
  QSharedPointer<DownloadJob> m_active;
  JobNotifier m_notifier;
  bool m_isOn = false;
};

#endif // JOBMANAGER_H
