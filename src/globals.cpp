/**
 * @file globals.cpp
 */

#include "globals.h"

// app settings
int g_themeId = 0;
bool g_darkMode = false;
QSettings* g_settings = nullptr;
QLocale::Language g_language;
QString g_updateToolPath;

// qcf fonts
int g_qcfVersion = 1;
QString g_qcfFontPrefix;
QString g_qcfBSMLFont;

// app directories
QDir g_themeResources;
QDir g_configDir = QDir::homePath();
QDir g_assetsDir;
QDir g_bismillahDir;
QDir g_recitationsDir;

// application specific
QList<Reciter> g_recitersList;
