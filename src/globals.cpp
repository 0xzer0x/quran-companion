/**
 * @file globals.cpp
 * @brief Instantiation of application-wide variables.
 */

#include "globals.h"
#include <QStandardPaths>

namespace Globals {
// app settings
int themeId = 0;
ReaderMode readerMode = ReaderMode::singlePage;
bool darkMode = false;
QSettings* settings = nullptr;
QLocale::Language language;

// qcf fonts
int qcfVersion = 1;
QString qcfFontPrefix;
QString qcfBSMLFont;

// app directories
QDir themeResources;
QDir assetsDir;
QDir bismillahDir;
QDir recitationsDir =
  QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
QDir configDir =
  QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

// application specific
QList<Reciter> recitersList;
QString updateToolPath;

};
