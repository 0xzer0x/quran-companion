#include "copynotifier.h"
#include <QCoreApplication>

CopyNotifier::CopyNotifier(QObject* parent)
{
  setParent(parent);
}

void
CopyNotifier::copied()
{
  QString msg = QCoreApplication::translate("CopyNotifier", "Verse text copied to clipboard");
  emit notify(copiedText, msg);
}
