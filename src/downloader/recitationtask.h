#ifndef RECITATIONTASK_H
#define RECITATIONTASK_H

#include <database/qurandb.h>
#include <interfaces/downloadtask.h>
#include <types/reciter.h>
#include <utils/dirmanager.h>

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
  const QuranDb& m_quranDb = QuranDb::getInstance();
  const QDir& m_downloadsDir = DirManager::getInstance().downloadsDir();
  const QList<Reciter>& m_reciters = Reciter::reciters;
  int m_reciter;
  int m_surah;
  int m_verse;
};

#endif // RECITATIONTASK_H
