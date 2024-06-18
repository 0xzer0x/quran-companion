#ifndef BOOKMARKSNOTIFIER_H
#define BOOKMARKSNOTIFIER_H

#include <notifiers/notificationsender.h>

class BookmarksNotifier : public NotificationSender
{
public:
  BookmarksNotifier(QObject* parent);
  void notifyAdded();
  void notifyRemoved();
};

#endif // BOOKMARKSNOTIFIER_H
