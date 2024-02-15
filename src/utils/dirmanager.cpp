#include "dirmanager.h"
#include <QApplication>
#include <QStandardPaths>

QSharedPointer<QDir> DirManager::fontsDir;
QSharedPointer<QDir> DirManager::assetsDir;
QSharedPointer<QDir> DirManager::basmallahDir;
QSharedPointer<QDir> DirManager::downloadsDir = QSharedPointer<QDir>::create(
  QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
QSharedPointer<QDir> DirManager::configDir = QSharedPointer<QDir>::create(
  QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));

void
DirManager::setup()
{

  assetsDir = QSharedPointer<QDir>::create(QApplication::applicationDirPath() +
                                           QDir::separator() + "assets");
  fontsDir = QSharedPointer<QDir>::create(assetsDir->absoluteFilePath("fonts"));
  basmallahDir = QSharedPointer<QDir>::create(
    QApplication::applicationDirPath() + QDir::separator() + "bismillah");

  // config & downloads
  if (!configDir->exists("QuranCompanion"))
    configDir->mkpath("QuranCompanion");
  configDir->cd("QuranCompanion");

  if (!downloadsDir->exists("QuranCompanion"))
    downloadsDir->mkpath("QuranCompanion");
  downloadsDir->cd("QuranCompanion");

  if (!downloadsDir->exists("recitations"))
    downloadsDir->mkpath("recitations");

  if (!downloadsDir->exists("QCFV2"))
    downloadsDir->mkpath("QCFV2");

  if (!downloadsDir->exists("tafasir"))
    downloadsDir->mkpath("tafasir");

  if (!downloadsDir->exists("translations"))
    downloadsDir->mkpath("translations");
}
