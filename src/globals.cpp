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
QString qcfFontPrefix;

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
QString updateToolPath;
QMap<QString, QString> shortcutDescription;
fa::QtAwesome* awesome;
};
