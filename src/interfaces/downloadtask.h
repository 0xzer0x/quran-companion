#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include <QFileInfo>
#include <QNetworkReply>
#include <QPointer>
#include <QUrl>

class DownloadTask
{
public:
  virtual QUrl url() const = 0;
  virtual QFileInfo destination() const = 0;
  virtual ~DownloadTask(){};
};

#endif // DOWNLOADTASK_H
