#ifndef JOBMANAGER_H
#define JOBMANAGER_H

#include "interfaces/downloadjob.h"
#include <QObject>
#include <QPointer>
#include <QQueue>

class JobManager : public QObject
{
  Q_OBJECT
public:
  explicit JobManager(QObject* parent);
  ~JobManager();

  void start();
  void stop();
  void addJob(QPointer<DownloadJob> job);

  bool isOn() const;
  QPointer<DownloadJob> active() const;

signals:
  void jobAborted();
  void jobCompleted(QPointer<DownloadJob> job);
  void jobFailed(QPointer<DownloadJob> job);
  void jobProgressed(QPointer<DownloadJob> job);
  void downloadSpeedUpdated(int speed, QString unit);
  void filesFound(QPointer<DownloadJob> job);

private slots:
  void handleProgressed();
  void handleFailed();
  void handleCompleted();
  void handleFilesFound();
  void processJobs();

private:
  void connectActive();
  void disconnectActive();
  QQueue<QPointer<DownloadJob>> m_queue;
  QPointer<DownloadJob> m_active;
  bool m_isOn = false;
};

#endif // JOBMANAGER_H
