/**
 * @file globals.cpp
 */

#include "globals.h"

// app settings
int Globals::themeId = 0;
bool Globals::darkMode = false;
QSettings* Globals::settings = nullptr;
QLocale::Language Globals::language;

// qcf fonts
int Globals::qcfVersion = 1;
QString Globals::qcfFontPrefix;
QString Globals::qcfBSMLFont;

// app directories
QDir Globals::themeResources;
QDir Globals::configDir = QDir::homePath();
QDir Globals::assetsDir;
QDir Globals::bismillahDir;
QDir Globals::recitationsDir;

// application specific
QList<Reciter> Globals::recitersList;
QString Globals::updateToolPath;
