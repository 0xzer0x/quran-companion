/**
 * @file globals.h
 * @brief Header file defining application-wide variables.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QMap>
#include <QSettings>
#include <QString>
#include <QtAwesome.h>

/**
 * @struct Verse
 * @brief Verse struct represents a single quran verse
 * @details Quran verses consist of 3 attributes. page (1-604). surah (1-114).
 * number represents the number of the verse in the surah (0-surah verse count).
 * Basmallah before the 1st verse is represented as verse number 0.
 */
struct Verse
{
  int page{ -1 };   ///< verse page
  int surah{ -1 };  ///< verse surah number
  int number{ -1 }; ///< verse number in surah
  bool operator==(const Verse& v2) const
  {
    return (this->number == v2.number && this->surah == v2.surah);
  }
  bool operator!=(const Verse& v2) const
  {
    return (this->number != v2.number || this->surah != v2.surah);
  }
  bool operator<(const Verse& v2) const
  {
    if (this->surah == v2.surah)
      return this->number < v2.number;

    return this->surah < v2.surah;
  }
  bool operator>(const Verse& v2) const
  {
    if (this->surah == v2.surah)
      return this->number > v2.number;

    return this->surah > v2.surah;
  }
};
/**
 * @struct Reciter
 * @brief Reciter struct represents a quran reciter
 */
struct Reciter
{
  QString baseDirName{}; ///< The name of the directory conatining recitations
                         ///< in the application recitations directory.
  QString displayName{}; ///< The reciter name as its displayed in the UI.
  QString
    basmallahPath{}; ///< Absolute path to the reciters bismillah audio file.
  QString baseUrl{}; ///< Url to download recitation files from.
  bool useId{
    false
  }; ///< Boolean value representing whether the verse recitations should be
     ///< downloading using the verse number relative to the beginning of the
     ///< Quran or a combination of surah and verse numbers.
};

enum VerseText
{
  qcf,
  uthmanic,
  annotated
};

/**
 * @brief ReaderMode enum represents the available modes for the Quran reader in
 * MainWindow
 */
enum ReaderMode
{
  SinglePage, ///< Single Quran page, side panel is used for displaying verses
              ///< with translation
  DoublePage  ///< Two Quran pages, both panels are used to display Quran pages,
              ///< no translation
};
/**
 * @brief Tafsir enum values representing available tafasir.
 */
enum Tafsir
{
  adwa,       ///< Adwa' ul-Bayan - Sheikh Shinqiti
  aysar,      ///< Aysar ul-Tafasir - Sheikh Abu-bakr al-Jazaeri
  baghawy,    ///< Tafsir ul-baghawy
  qortoby,    ///< Tafsir ul-Qortoby
  sa3dy,      ///< Tafsir ul-Sa'ady
  tabary,     ///< Tafsir ul-Tabary
  waseet,     ///< At-Tafsir ul-Waseet
  jalalayn,   ///< Tafsir ul-Jalalyn
  e3rab,      ///< Ea'rab of Quran verses
  tanweer,    ///< At-Tahreer wa At-Tanweer - Sheikh Ibn A'ashoor
  juzayy,     ///< Tafsir Ibn-Juzayy
  katheer,    ///< Tafsir Ibn-Katheer
  katheer_en, ///< (English) Tafsir Ibn-Katheer
  indonesian, ///< (Indonesian) Tafsir ul-Jalalyn
  russian,    ///< (Russian) Tafsir ul-Sa'ady
  tafheem     ///< (English) Tafheem ul-Quran - Sheikh Abul-Aa'la al-Maududy
};
/**
 * @brief Translation enum holds different values representing available Quran
 * translations
 */
enum Translation
{
  muyassar,       ///< (Arabic) Tafsir muyassar
  ar_ma3any,      ///< (Arabic) Word Meanings
  bn_bengali,     ///< (Bengali) Muhiuddin Khan
  bs_korkut,      ///< (Bosanski) Korkut
  de_bubenheim,   ///< (Deutsch) Bubenheim & Elyas
  en_khattab,     ///< (English) Clear Quran
  en_sahih,       ///< (English) Sahih International
  en_yusuf,       ///< (English) Abdullah Yusuf Ali
  es_navio,       ///< (Español) Abdel Ghani Navio
  fr_hamidullah,  ///< (Français) Hamidullah
  ha_gumi,        ///< (Hausa) Gumi
  id_indonesian,  ///< (Indonesian) Bahasa Indonesia
  it_piccardo,    ///< (Italiano) Piccardo
  ku_asan,        ///< (كوردى) برهان محمد أمين
  ml_abdulhameed, ///< (Malayalam) Abdul Hameed and Kunhi
  ms_basmeih,     ///< (Melayu) Basmeih
  nl_siregar,     ///< (Dutch) Sofian Siregar
  pr_tagi,        ///< (فارسى) حسین تاجی گله داری
  pt_elhayek,     ///< (Português) El Hayek
  ru_kuliev,      ///< (Русский) Кулиев (Russian - Elmir Kuliev)
  so_abduh,       ///< (Somali) Abduh
  sq_nahi,        ///< (Shqiptar) Efendi Nahi
  sv_bernstrom,   ///< (Swedish) Bernström
  sw_barwani,     ///< (Swahili) Al-Barwani
  ta_tamil, ///< (தமிழ்) ஜான் டிரஸ்ட் (Tamil - Jan Turst Foundation)
  th_thai,  ///< (ภาษาไทย) ภาษาไทย (Thai)
  tr_diyanet,   ///< (Türkçe) Diyanet Isleri
  ur_jalandhry, ///< (اردو) جالندربرى
  uz_sodik,     ///< (Uzbek) Мухаммад Содик
  zh_jian       ///< (中国语文) Ma Jian (Chinese)
};

namespace Globals {
extern int
  themeId; ///< global variable represnting the application theme index.

extern QSettings*
  settings; ///< global pointer to the application QSettings instance.

extern bool
  darkMode; ///< global boolean to indicate if application is in dark mode.

extern QLocale::Language
  language; ///< global QLocale::Language instance for application languge.

extern QString
  updateToolPath; ///< global absolute path for the application update tool.

extern int qcfVersion; ///< global variable for the QCF version in use.

extern ReaderMode readerMode;

extern QString qcfFontPrefix; ///< global variable for the QCF font prefix to
                              ///< generate font name from.

extern QString
  qcfBSMLFont; ///< global variable for the name of the QCF basmallah font.

extern QDir themeResources; ///< global QDir for the current theme resources
                            ///< (icons & styles).
extern QDir
  configDir; ///< global QDir representing application config directiory.

extern QDir assetsDir; ///< global QDir representing the application assets
                       ///< directory (fonts, translations, tafsir).

extern QDir
  bismillahDir; ///< global QDir representing the reciters basmallah files.

extern QDir downloadsDir; ///< global QDir representing the top-level path
                          ///< for downloaded files.

extern QList<Reciter> recitersList; ///< global QList containing reciters
                                    ///< supported by the application.

extern QMap<QString, QString>
  shortcutDescription; ///< global QMap containing all available
                       ///< application shortcuts as keys and their descriptions
                       ///< as values.

extern QObject* databaseManager; ///< global pointer to the application-wide
                                 ///< DBManager instance for interacting with
                                 ///< different database files.

extern fa::QtAwesome*
  awesome; ///< global pointer used for generating font awesome icons

};

#endif // GLOBALS_H
