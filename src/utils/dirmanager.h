#ifndef DIRMANAGER_H
#define DIRMANAGER_H

#include <QDir>
#include <QSharedPointer>

class DirManager
{
public:
  static void setup();
  static QSharedPointer<QDir> fontsDir;
  static QSharedPointer<QDir> configDir;
  static QSharedPointer<QDir> assetsDir;
  static QSharedPointer<QDir> downloadsDir;
  static QSharedPointer<QDir> basmallahDir;
};

#endif // DIRMANAGER_H
