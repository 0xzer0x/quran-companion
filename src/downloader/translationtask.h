#ifndef TRANSLATIONTASK_H
#define TRANSLATIONTASK_H

#include <interfaces/downloadtask.h>
#include <types/translation.h>
#include <utils/dirmanager.h>

class TranslationTask : public DownloadTask
{
public:
  TranslationTask(int idx);
  TranslationTask(const TranslationTask& other);
  TranslationTask(const TranslationTask&& other);

  TranslationTask& operator=(const TranslationTask other);

  QUrl url() const override;
  QFileInfo destination() const override;

private:
  const QDir& m_downloadsDir = DirManager::getInstance().downloadsDir();
  const QList<Translation>& m_translations =
    Translation::translations;
  int m_idx;
};

#endif // TRANSLATIONTASK_H
