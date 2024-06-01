#ifndef QCFTASK_H
#define QCFTASK_H

#include <downloader/downloadtask.h>
#include <utils/dirmanager.h>

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
  const QDir& m_downloadsDir = DirManager::getInstance().downloadsDir();
  int m_page;
};

#endif // QCFTASK_H
