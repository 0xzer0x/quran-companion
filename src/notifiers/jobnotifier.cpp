#include "jobnotifier.h"
#include <QApplication>

JobNotifier::JobNotifier(QObject* parent)
{
  setParent(parent);
}

void
JobNotifier::notifyCompleted(QPointer<DownloadJob> job)
{
  QString msg =
    qApp->translate("JobNotifier", "Download Completed") + ": " + job->name();
  emit notify(success, msg);
}

void
JobNotifier::notifyFailed(QPointer<DownloadJob> job)
{
  QString msg =
    qApp->translate("JobNotifier", "Download Failed") + ": " + job->name();
  emit notify(fail, msg);
}
