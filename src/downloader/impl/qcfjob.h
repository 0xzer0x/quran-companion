#ifndef QCFJOB_H
#define QCFJOB_H

#include "taskdownloader.h"
#include <QQueue>
#include <downloader/downloadjob.h>
#include <downloader/impl/qcftask.h>

class QcfJob : public DownloadJob
{
  Q_OBJECT
public:
  QcfJob();
  ~QcfJob();

  void start() override;
  void stop() override;
  bool isDownloading() override;
  int completed() override;
  int total() override;
  Type type() override;
  QString name() override;

  void enqueueTasks();

private slots:
  void processTasks();
  void taskFinished();
  void taskFailed();

private:
  TaskDownloader m_taskDlr;
  QQueue<QcfTask> m_queue;
  QNetworkAccessManager m_netMgr;
  QcfTask m_active;
  bool m_isDownloading;
  int m_completed;
};

#endif // QCFJOB_H
