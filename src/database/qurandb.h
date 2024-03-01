#ifndef QURANDB_H
#define QURANDB_H

#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <interfaces/dbconnection.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>
typedef Configuration::VerseType VerseType;

class QuranDb
  : public DbConnection
  , QSqlDatabase
{
public:
  static QuranDb& getInstance();
  void open();
  Type type();
  /**
   * @brief gets the surah number and juz number of the first verse in the page,
   * used to display page header information
   * @param page - Quran page number
   * @return QList of 2 integers [0: surah index, 1: juz number]
   */
  QPair<int, int> pageMetadata(const int page) const;
  /**
   * @brief gets the page where the verse is found
   * @param surahIdx - sura number
   * @param verse - verse number
   * @return page number
   */
  int getVersePage(const int& surahIdx, const int& verse) const;
  /**
   * @brief gets the page where the corresponding juz starts
   * @param juz - juz number
   * @return page number
   */
  int getJuzStartPage(const int juz) const;
  /**
   * @brief get the juz which the passed page is a part of
   * @param page - page number
   * @return juz number
   */
  int getJuzOfPage(const int page) const;
  /**
   * @brief gets a QList of ::Verse instances for the page verses
   * @param page - Quran page number
   * @return QList of ::Verse instances
   */
  QList<QList<int>> verseInfoList(const int page) const;
  /**
   * @brief gets the verse text
   * @param sIdx - sura number (1-114)
   * @param vIdx - verse number
   * @return QString of the verse text
   */
  QString verseText(const int sIdx, const int vIdx) const;
  /**
   * @brief gets the page where the surah begins
   * @param surahIdx - sura number
   * @return page of the first verse in the sura
   */
  int surahStartPage(int surahIdx) const;
  /**
   * @brief gets the surah name in English or Arabic (default is English)
   * @param sIdx - sura number
   * @param ar - boolean to return arabic sura name
   * @return QString containing the sura name
   */
  QString surahName(const int sIdx, bool ar = false) const;
  /**
   * @brief get the verse with the corresponding id and return it as a ::Verse
   * instance
   * @param id - verse id
   * @return ::Verse instance
   */
  QList<int> verseById(const int id) const;
  /**
   * @brief gets the page where the verse is found
   * @param surahIdx - sura number
   * @param verse - verse number
   * @return page number
   */
  int versePage(const int& surahIdx, const int& verse) const;
  /**
   * @brief searches the database for surahs matching the given text pattern,
   * the pattern can be either in English or Arabic
   * @param text - name / part of the name of the sura
   * @return QList of sura numbers which contain the given text
   */
  QList<int> searchSurahNames(QString text) const;
  /**
   * @brief search specific surahs for the given search text
   * @param searchText - text to search for
   * @param surahs - QList of surah numbers to search in
   * @param whole - boolean value to search for whole words only
   * @return QList of ::Verse instances representing the search results
   */
  QList<QList<int>> searchSurahs(QString searchText,
                                 const QList<int> surahs,
                                 const bool whole = false) const;
  /**
   * @brief search a range of pages for the given search text
   * @param searchText - text to search for
   * @param range - array of start & end page numbers
   * @param whole - boolean value to indicate search for whole words only
   * @return QList of ::Verse instances representing the search results
   */
  QList<QList<int>> searchVerses(QString searchText,
                                 const int range[2] = new int[2]{ 1, 604 },
                                 const bool whole = false) const;
  /**
   * @brief gets a random verse from the Quran
   * @return QPair of ::Verse instance and verse text
   */
  QList<int> randomVerse() const;
  /**
   * @brief setVerseType
   * @param newVerseType
   */
  void setVerseType(VerseType newVerseType);
  /**
   * @brief verseType
   * @return
   */
  VerseType verseType() const;
  /**
   * @brief surahNames
   * @return
   */
  QStringList surahNames() const;

private:
  QuranDb();
  Configuration& m_config;
  const QDir& m_assetsDir;
  VerseType m_verseType;
  /**
   * @brief QList of sura names (Arabic if UI language is Arabic, Otherwise
   * English)
   */
  QStringList m_surahNames;
};

#endif // QURANDB_H
