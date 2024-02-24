#ifndef TRANSLATIONJOB_H
#define TRANSLATIONJOB_H

#include "../types/translation.h"
#include "../types/translationtask.h"
#include "downloadjob.h"
#include "taskdownloader.h"

class TranslationJob : public DownloadJob
{
public:
  TranslationJob(int idx);
  ~TranslationJob();

  void start() override;
  void stop() override;
  bool isDownloading() override;
  int completed() override;
  int total() override;
  Type type() override;
  QString name() override;

private:
  QList<QSharedPointer<Translation>>& m_translations =
    Translation::translations;
  QPointer<TaskDownloader> m_taskDlr;
  QNetworkAccessManager m_netMgr;
  TranslationTask m_task;
  bool m_isDownloading;
  int m_idx;
};

#endif // TRANSLATIONJOB_H
