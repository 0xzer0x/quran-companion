#ifndef RECITATIONTASK_H
#define RECITATIONTASK_H

#include <downloader/downloadtask.h>
#include <repository/quranrepository.h>
#include <service/quranservice.h>
#include <types/reciter.h>
#include <utils/dirmanager.h>
#include <service/servicefactory.h>

class RecitationTask : public DownloadTask
{
public:
  RecitationTask();
  RecitationTask(int reciter, int surah, int verse);
  RecitationTask(const RecitationTask& other);
  RecitationTask(const RecitationTask&& other);
  ~RecitationTask();

  RecitationTask& operator=(RecitationTask other);

  QUrl url() const override;
  QFileInfo destination() const override;

private:
  const QDir& m_downloadsDir = DirManager::getInstance().downloadsDir();
  const QList<Reciter>& m_reciters = Reciter::reciters;
  int m_reciter;
  int m_surah;
  int m_verse;
};

#endif // RECITATIONTASK_H
