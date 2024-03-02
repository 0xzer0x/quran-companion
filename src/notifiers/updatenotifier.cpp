#include "updatenotifier.h"
#include <QApplication>

UpdateNotifier::UpdateNotifier(QObject* parent) {}

void
UpdateNotifier::notifyUpdate(QString version)
{
  QString msg =
    qApp->translate("UpdateNotifier", "Update available") + ": " + version;
  emit notify(updateInfo, msg);
}

void
UpdateNotifier::notifyLatest()
{
  QString msg =
    qApp->translate("UpdateNotifier", "You are running the latest version");
  emit notify(success, msg);
}
