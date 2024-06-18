#ifndef JOBNOTIFIER_H
#define JOBNOTIFIER_H

#include <QPointer>
#include <downloader/downloadjob.h>
#include <notifiers/notificationsender.h>

class JobNotifier : public NotificationSender
{
public:
  JobNotifier(QObject* parent);
  void notifyCompleted(QPointer<DownloadJob> job);
  void notifyFailed(QPointer<DownloadJob> job);
};

#endif // JOBNOTIFIER_H
