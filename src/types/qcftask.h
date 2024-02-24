#ifndef QCFTASK_H
#define QCFTASK_H

#include "../utils/dirmanager.h"
#include "downloadtask.h"

class QcfTask : public DownloadTask
{
public:
  QcfTask(int page);
  QcfTask(const QcfTask& other);
  QcfTask(const QcfTask&& other);

  QcfTask& operator=(const QcfTask other);

  QUrl url() const override;
  QFileInfo destination() const override;

private:
  QSharedPointer<const QDir> m_downloadsDir = DirManager::downloadsDir;
  int m_page;
};

#endif // QCFTASK_H
