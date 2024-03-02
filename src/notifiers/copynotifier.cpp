#include "copynotifier.h"
#include <QApplication>

CopyNotifier::CopyNotifier(QObject* parent)
{
  setParent(parent);
}

void
CopyNotifier::copied()
{
  QString msg =
    qApp->translate("CopyNotifier", "Verse text copied to clipboard");
  emit notify(copiedText, msg);
}
