#include "copynotifier.h"

CopyNotifier::CopyNotifier(QObject* parent)
{
  setParent(parent);
}

void
CopyNotifier::copied()
{
  QString msg = tr("Verse text copied to clipboard");
  emit notify(copiedText, msg);
}
