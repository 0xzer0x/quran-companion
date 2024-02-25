#ifndef SURAHJOB_H
#define SURAHJOB_H

#include "interfaces/downloadjob.h"
#include "recitationtask.h"
#include "taskdownloader.h"
#include <QQueue>
#include <QTime>

class SurahJob : public DownloadJob
{
  Q_OBJECT
public:
  SurahJob(int reciter, int surah);
  ~SurahJob();

  void start() override;
  void stop() override;
  bool isDownloading() override;
  int completed() override;
  int total() override;
  Type type() override;
  QString name() override;

  void enqueueTasks();
  int reciter() const;
  int surah() const;

private slots:
  void processTasks();
  void taskFinished();
  void taskFailed();

private:
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  QList<QSharedPointer<Reciter>>& m_reciters = Reciter::reciters;
  TaskDownloader m_taskDlr;
  QQueue<RecitationTask> m_queue;
  QNetworkAccessManager m_netMgr;
  RecitationTask m_active;
  bool m_isDownloading;
  int m_reciter;
  int m_surah;
  int m_completed;
  int m_surahCount;
};

#endif // SURAHJOB_H
