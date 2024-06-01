#ifndef TAFSIRTASK_H
#define TAFSIRTASK_H

#include <downloader/downloadtask.h>
#include <types/tafsir.h>
#include <utils/dirmanager.h>

class TafsirTask : public DownloadTask
{
public:
  TafsirTask(int idx);
  TafsirTask(const TafsirTask& other);
  TafsirTask(const TafsirTask&& other);

  TafsirTask& operator=(const TafsirTask other);

  QUrl url() const override;
  QFileInfo destination() const override;

private:
  const QDir& m_downloadsDir = DirManager::getInstance().downloadsDir();
  const QList<Tafsir>& m_tafasir = Tafsir::tafasir;
  int m_idx;
};

#endif // TAFSIRTASK_H
