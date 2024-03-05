#ifndef CONTENTJOB_H
#define CONTENTJOB_H

#include "taskdownloader.h"
#include <interfaces/downloadjob.h>
#include <types/tafsir.h>
#include <types/translation.h>

class ContentJob : public DownloadJob
{
  Q_OBJECT
public:
  ContentJob(Type type, int idx);
  ~ContentJob();

  void start() override;
  void stop() override;
  bool isDownloading() override;
  int completed() override;
  int total() override;
  Type type() override;
  QString name() override;

signals:
  void fileFound();

private:
  QList<Tafsir>& m_tafasir;
  QList<Translation>& m_translations;
  TaskDownloader m_taskDlr;
  QNetworkAccessManager m_netMgr;
  DownloadTask* m_task;
  Type m_type;
  bool m_isDownloading;
  int m_idx;
};

#endif // CONTENTJOB_H
