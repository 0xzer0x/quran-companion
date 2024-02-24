#ifndef TRANSLATIONTASK_H
#define TRANSLATIONTASK_H

#include "interfaces/downloadtask.h"
#include "types/translation.h"
#include "utils/dirmanager.h"

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
  QSharedPointer<const QDir> m_downloadsDir = DirManager::downloadsDir;
  const QList<QSharedPointer<Translation>>& m_translations =
    Translation::translations;
  int m_idx;
};

#endif // TRANSLATIONTASK_H
