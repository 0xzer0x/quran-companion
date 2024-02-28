#include "jobnotifier.h"

JobNotifier::JobNotifier(QObject* parent)
{
  setParent(parent);
}

void
JobNotifier::notifyCompleted(QPointer<DownloadJob> job)
{
  QString msg = tr("Download Completed") + ": " + job->name();
  emit notify(success, msg);
}

void
JobNotifier::notifyFailed(QPointer<DownloadJob> job)
{
  QString msg = tr("Download Failed") + ": " + job->name();
  emit notify(fail, msg);
}
