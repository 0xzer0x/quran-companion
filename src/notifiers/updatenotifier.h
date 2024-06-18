#ifndef UPDATENOTIFIER_H
#define UPDATENOTIFIER_H

#include <notifiers/notificationsender.h>

class UpdateNotifier : public NotificationSender
{
public:
  UpdateNotifier(QObject* parent);
  void notifyUpdate(QString version);
  void notifyLatest();
};

#endif // UPDATENOTIFIER_H
