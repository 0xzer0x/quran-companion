#ifndef QURANDB_H
#define QURANDB_H

#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <interfaces/dbconnection.h>
#include <utils/dirmanager.h>
#include <utils/settings.h>
typedef Settings::VerseType VerseType;

class QuranDb
  : public DbConnection
  , QSqlDatabase
{
public:
    static QSharedPointer<QuranDb> current();
  QuranDb();
  void open();
  Type type();
  /**
   * @brief gets the surah number and juz number of the first verse in the page,
   * used to display page header information
   * @param page - Quran page number
   * @return QList of 2 integers [0: surah index, 1: juz number]
   */
  QPair<int, int> pageMetadata(const int page);
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
   * @brief gets a QList of ::Verse instances for the page verses
   * @param page - Quran page number
   * @return QList of ::Verse instances
   */
  QList<QList<int>> verseInfoList(const int page);
  /**
   * @brief gets the verse text
   * @param sIdx - sura number (1-114)
   * @param vIdx - verse number
   * @return QString of the verse text
   */
  QString verseText(const int sIdx, const int vIdx);
  /**
   * @brief gets the number of the last verse in the surah passed
   * @param surahIdx - surah number (1-114)
   * @return number of verses in the sura
   */
  int surahVerseCount(const int surahIdx);
  /**
   * @brief gets the page where the surah begins
   * @param surahIdx - sura number
   * @return page of the first verse in the sura
   */
  int surahStartPage(int surahIdx);
  /**
   * @brief gets the surah name in English or Arabic (default is English)
   * @param sIdx - sura number
   * @param ar - boolean to return arabic sura name
   * @return QString containing the sura name
   */
  QString surahName(const int sIdx, bool ar = false);
  /**
   * @brief gets the corresponding id for the verse in the database
   * @param sIdx - sura number
   * @param vIdx - verse number
   * @return id of the verse
   */
  int verseId(const int sIdx, const int vIdx);
  /**
   * @brief get the verse with the corresponding id and return it as a ::Verse
   * instance
   * @param id - verse id
   * @return ::Verse instance
   */
  QList<int> verseById(const int id);
  /**
   * @brief gets the page where the verse is found
   * @param surahIdx - sura number
   * @param verse - verse number
   * @return page number
   */
  int versePage(const int& surahIdx, const int& verse);
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
  QList<QList<int>> searchSurahs(QString searchText,
                                 const QList<int> surahs,
                                 const bool whole = false);
  /**
   * @brief search a range of pages for the given search text
   * @param searchText - text to search for
   * @param range - array of start & end page numbers
   * @param whole - boolean value to indicate search for whole words only
   * @return QList of ::Verse instances representing the search results
   */
  QList<QList<int>> searchVerses(QString searchText,
                                 const int range[2] = new int[2]{ 1, 604 },
                                 const bool whole = false);
  /**
   * @brief gets a random verse from the Quran
   * @return QPair of ::Verse instance and verse text
   */
  QList<int> randomVerse();
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
  const int m_qcfVer = Settings::qcfVersion;
  const QLocale::Language m_languageCode = Settings::language;
  const QSharedPointer<QDir> m_assetsDir = DirManager::assetsDir;
  VerseType m_verseType = Settings::Qcf;
  /**
   * @brief QList of sura names (Arabic if UI language is Arabic, Otherwise
   * English)
   */
  QStringList m_surahNames;
};

#endif // QURANDB_H
