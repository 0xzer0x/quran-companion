#ifndef COPYNOTIFIER_H
#define COPYNOTIFIER_H

#include <notifiers/notificationsender.h>

class CopyNotifier : public NotificationSender
{
public:
  CopyNotifier(QObject* parent);
  void copied();
};

#endif // COPYNOTIFIER_H
