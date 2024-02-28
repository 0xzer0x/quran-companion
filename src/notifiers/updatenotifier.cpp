#include "updatenotifier.h"

UpdateNotifier::UpdateNotifier(QObject* parent) {}

void
UpdateNotifier::notifyUpdate(QString version)
{
  QString msg = tr("Update available") + ": " + version;
  emit notify(updateInfo, msg);
}

void
UpdateNotifier::notifyLatest()
{
  QString msg = tr("You are running the latest version");
  emit notify(success, msg);
}
