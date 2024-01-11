/**
 * @file globals.cpp
 * @brief Instantiation of application-wide variables.
 */

#include "globals.h"
#include <QStandardPaths>

namespace Globals {
// app settings
int themeId = 0;
ReaderMode readerMode = ReaderMode::SinglePage;
bool darkMode = false;
QSettings* settings = nullptr;
QObject* databaseManager = nullptr;
QLocale::Language language;

// qcf fonts
int qcfVersion = 1;
QString qcfFontPrefix = "QCF_P";

// app directories
QDir themeResources;
QDir assetsDir;
QDir bismillahDir;
QDir downloadsDir =
  QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
QDir configDir =
  QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

// application specific
QList<Reciter> recitersList;
QList<Tafsir> tafasirList;
QList<Translation> translationsList;
QString updateToolPath;
QMap<QString, QString> shortcutDescription;
fa::QtAwesome* awesome;

QString
pageFontname(int page)
{
  return qcfFontPrefix + QString::number(page).rightJustified(3, '0');
}

QString
verseFontname(VerseType type, int page)
{
  QString fontname;
  switch (type) {
    case qcf:
      fontname = pageFontname(page);
      break;
    case uthmanic:
      fontname = "kfgqpc_hafs_uthmanic _script";
      break;
    case annotated:
      fontname = "Emine";
      break;
  }
  return fontname;
}

};
