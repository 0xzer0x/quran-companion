﻿/**
 * @file dbmanager.h
 * @brief Header file for DBManager
 */

#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "../globals.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QObject>
#include <QRandomGenerator>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

/**
 * @brief DBManager is as an interface for preforming queries to
 * different database files holding application data.
 */
class DBManager : public QObject
{
  Q_OBJECT

public:
  /**
   * @brief Database enum holds different values representing database files
   * used in different member functions.
   */
  enum Database
  {
    null,       ///< default value
    quran,      ///< (quran.db) main Quran database file
    glyphs,     ///< (glyphs.db) QCF glyphs database
    bookmarks,  ///< (bookmarks.db) bookmarked verses and khatmah database
    tafsir,     ///< currently selected tafsir database file
    translation ///< currently selected translation database file
  };

  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   */
  explicit DBManager(QObject* parent = nullptr);
  /**
   * @brief getter for translated Surah names QList
   * @return QList of Surah name strings
   */
  QList<QString> surahNameList();
  /**
   * @brief sets the active tafsir
   * @param tafsirName - DBManager::Tafsir entry
   */
  void setCurrentTafsir(Tafsir tafsirName);
  /**
   * @brief sets the active translation
   * @param translationName - DBManager::Translation entry
   */
  void setCurrentTranslation(Translation translationName);
  /**
   * @brief sets the currently active sqlite database file and opens
   * corresponding connection based on the DBManager::Database type
   * @param db - DBManager::Database entry
   * @param filePath - path to the database file
   */
  void setOpenDatabase(Database db, QString filePath);
  /**
   * @brief gets the surah number and juz number of the first verse in the page,
   * used to display page header information
   * @param page - Quran page number
   * @return QList of 2 integers [0: surah index, 1: juz number]
   */
  QList<int> getPageMetadata(const int page);
  /**
   * @brief get Quran page QCF glyphs separated as lines
   * @param page - Quran page number
   * @return QList of page lines
   */
  QStringList getPageLines(const int page);
  /**
   * @brief gets a QList of ::Verse instances for the page verses
   * @param page - Quran page number
   * @return QList of ::Verse instances
   */
  QList<Verse> getVerseInfoList(const int page);
  /**
   * @brief gets the surah name glyph for the QCF_BSML font, used to render
   * surah frame in Quran page
   * @param sura - sura number (1-114)
   * @return QString of glyphs
   */
  QString getSurahNameGlyph(const int sura);
  /**
   * @brief gets the juz name in arabic, used in page header
   * @param juz - juz number
   * @return QString of the juz name
   */
  QString getJuzGlyph(const int juz);
  /**
   * @brief gets the verse QCF glyphs for the corresponding QCF page font
   * @param sIdx - sura number (1-114)
   * @param vIdx - verse number
   * @return QString of verse glyphs
   */
  QString getVerseGlyphs(const int sIdx, const int vIdx);
  /**
   * @brief gets the verse text
   * @param sIdx - sura number (1-114)
   * @param vIdx - verse number
   * @return QString of the verse text
   */
  QString getVerseText(const int sIdx, const int vIdx);
  /**
   * @brief sets the given ::Verse as the last position reached in the current
   * active khatmah
   * @param v - ::Verse reached in khatmah
   */
  bool saveActiveKhatmah(const Verse& v);
  /**
   * @brief get all available khatmah ids
   * @return QList of khatmah id(s)
   */
  QList<int> getAllKhatmah();
  /**
   * @brief get the name of the khatmah with id given
   * @return QString containing the khatmah name
   */
  QString getKhatmahName(const int id);
  /**
   * @brief gets the last position saved for the khatmah with the id given and
   * stores the position in the ::Verse v
   * @return boolean indicating a successful operation (false in case of error
   * and in case id does not exist)
   */
  bool getKhatmahPos(const int khatmahId, Verse& v);
  /**
   * @brief add a new khatmah/replace khatmah with given id with position of
   * ::Verse v
   * @param v - ::Verse to set as the khatmah position
   * @param name - new khatmah name
   * @param id - id of khatmah to replace, -1 means do not replace (default: -1)
   * @return id of newly added khatmah or id parameter if defined
   */
  int addKhatmah(const Verse& v, const QString name, const int id = -1);
  /**
   * @brief rename the khatmah with the given id to newName
   * @param khatmahId - id of khatmah to rename
   * @param newName - new name to set
   * @return boolean indicating a successful operation (false in case the name
   * exists)
   */
  bool editKhatmahName(const int khatmahId, QString newName);
  /**
   * @brief remove the khatmah with the given id from database
   * @param id - id of khatmah to remove
   */
  void removeKhatmah(const int id);
  /**
   * @brief gets the number of the last verse in the surah passed
   * @param surahIdx - surah number (1-114)
   * @return number of verses in the sura
   */
  int getSurahVerseCount(const int surahIdx);
  /**
   * @brief gets the page where the surah begins
   * @param surahIdx - sura number
   * @return page of the first verse in the sura
   */
  int getSurahStartPage(int surahIdx);
  /**
   * @brief gets the surah name in English or Arabic (default is English)
   * @param sIdx - sura number
   * @param ar - boolean to return arabic sura name
   * @return QString containing the sura name
   */
  QString getSurahName(const int sIdx, bool ar = false);
  /**
   * @brief gets the corresponding id for the verse in the database
   * @param sIdx - sura number
   * @param vIdx - verse number
   * @return id of the verse
   */
  int getVerseId(const int sIdx, const int vIdx);
  /**
   * @brief get the verse with the corresponding id and return it as a ::Verse
   * instance
   * @param id - verse id
   * @return ::Verse instance
   */
  Verse getVerseById(const int id);
  /**
   * @brief gets the page where the verse is found
   * @param surahIdx - sura number
   * @param verse - verse number
   * @return page number
   */
  int getVersePage(const int& surahIdx, const int& verse);
  /**
   * @brief gets the page where the corresponding juz starts
   * @param juz - juz number
   * @return page number
   */
  int getJuzStartPage(const int juz);
  /**
   * @brief get the juz which the passed page is a part of
   * @param page - page number
   * @return juz number
   */
  int getJuzOfPage(const int page);
  /**
   * @brief searches the database for surahs matching the given text pattern,
   * the pattern can be either in English or Arabic
   * @param text - name / part of the name of the sura
   * @return QList of sura numbers which contain the given text
   */
  QList<int> searchSurahNames(QString text);
  /**
   * @brief search specific surahs for the given search text
   * @param searchText - text to search for
   * @param surahs - QList of surah numbers to search in
   * @param whole - boolean value to search for whole words only
   * @return QList of ::Verse instances representing the search results
   */
  QList<Verse> searchSurahs(QString searchText,
                            const QList<int> surahs,
                            const bool whole = false);
  /**
   * @brief search a range of pages for the given search text
   * @param searchText - text to search for
   * @param range - array of start & end page numbers
   * @param whole - boolean value to indicate search for whole words only
   * @return QList of ::Verse instances representing the search results
   */
  QList<Verse> searchVerses(QString searchText,
                            const int range[2] = new int[2]{ 1, 604 },
                            const bool whole = false);
  /**
   * @brief gets the tafsir content for the given verse using the active
   * DBManager::Tafsir
   * @param sIdx - surah number
   * @param vIdx - verse number
   * @return QString containing the tafsir of the verse
   */
  QString getTafsir(const int sIdx, const int vIdx);
  /**
   * @brief gets the translation of the given verse using the active
   * DBManager::Translation
   * @param sIdx - surah number
   * @param vIdx - verse number
   * @return QString containing the verse translation
   */
  QString getTranslation(const int sIdx, const int vIdx);
  /**
   * @brief gets a random verse from the Quran
   * @return QPair of ::Verse instance and verse text
   */
  QPair<Verse, QString> randomVerse();
  /**
   * @brief gets a QList of ::Verse instances representing the bookmarked verse
   * within the given sura (default gets all)
   * @param surahIdx - sura number (-1 returns all bookmarks)
   * @return QList of bookmarked verses
   */
  QList<Verse> bookmarkedVerses(int surahIdx = -1);
  /**
   * @brief checks whether the given ::Verse is bookmarked
   * @param v - ::Verse instance to check
   * @return boolean
   */
  bool isBookmarked(Verse v);
  /**
   * @brief add the given ::Verse to bookmarks
   * @param v - ::Verse instance to add
   * @return boolean
   */
  bool addBookmark(Verse v);
  /**
   * @brief remove the given ::Verse from bookmarks
   * @param v - ::Verse instance to remove
   * @return boolean indicating successful removal
   */
  bool removeBookmark(Verse v);
  /**
   * @brief getter for m_currTafsir
   * @return the currently set DBManager::Tafsir
   */
  Tafsir currTafsir() const;
  /**
   * @brief getter for m_activeKhatmah
   * @return the currently active khatmah id
   */
  const int activeKhatmah() const;
  /**
   * @brief setter for m_activeKhatmah
   * @param id - id of the active khatmah
   */
  void setActiveKhatmah(const int id);

private:
  QDir m_dbDir = Globals::assetsDir;
  const QSettings* m_settings = Globals::settings;
  const QLocale::Language m_languageCode = Globals::language;
  const int m_qcfVer = Globals::qcfVersion;
  const QString m_bookmarksFilepath =
    Globals::configDir.absoluteFilePath("bookmarks.db");
  /**
   * @brief integer id of the current active khatmah
   */
  int m_activeKhatmah = 0;
  /**
   * @brief the currently active database type
   */
  Database m_currentDb = null;
  /**
   * @brief QSqlDatabase instance to interact with the different sqlite
   * databases
   */
  QSqlDatabase m_openDBCon;
  /**
   * @brief the current active DBManager::Tafsir
   */
  Tafsir m_currTafsir = Tafsir::sa3dy;
  /**
   * @brief the current active DBManager::Translation
   */
  Translation m_currTrans = Translation::en_sahih;
  /**
   * @brief the currently active tafsir database filename
   */
  QString m_tafsirDbFilename;
  /**
   * @brief the currently active translation database filename
   */
  QString m_transDbFilename;
  /**
   * @brief path to the currently active tafsir database file
   */
  QFileInfo m_tafsirDbPath;
  /**
   * @brief path to the currently active translation database file
   */
  QFileInfo m_transDbPath;
  /**
   * @brief path to the Quran database file
   */
  QFileInfo m_quranDbPath;
  /**
   * @brief path to the QCF glyphs database file
   */
  QFileInfo m_glyphsDbPath;
  /**
   * @brief QList of sura names (Arabic if UI language is Arabic, Otherwise
   * English)
   */
  QStringList m_surahNames;
};

#endif // DBMANAGER_H
