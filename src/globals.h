#ifndef GLOBALS_H
#define GLOBALS_H

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QSettings>
#include <QString>

struct Verse
{
  int page{ -1 };
  int surah{ -1 };
  int number{ -1 };
  bool operator==(const Verse& v2)
  {
    return (this->number == v2.number && this->surah == v2.surah);
  }
  bool operator!=(const Verse& v2)
  {
    return (this->number != v2.number || this->surah != v2.surah);
  }
};
struct Reciter
{
  QString baseDirName{};
  QString displayName{};
  QString basmallahPath{};
  QString baseUrl{};
  bool useId{ false };
};

extern int g_themeId;
extern QSettings* g_settings;
extern QLocale::Language g_language;
extern bool g_darkMode;
extern QString g_updateToolPath;

extern int g_qcfVersion;
extern QString g_qcfFontPrefix;
extern QString g_qcfBSMLFont;

extern QDir g_themeResources;
extern QDir g_configDir;
extern QDir g_assetsDir;
extern QDir g_bismillahDir;
extern QDir g_recitationsDir;

extern QList<Reciter> g_recitersList;

#endif // GLOBALS_H
