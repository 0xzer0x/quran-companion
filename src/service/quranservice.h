#ifndef QURANSERVICE_H
#define QURANSERVICE_H

#include <QList>
#include <QPair>

#include <types/verse.h>

class QuranService
{
public:
  /**
   * @brief gets the surah number and juz number of the first verse in the page,
   * used to display page header information
   * @param page - Quran page number
   * @return QList of 2 integers [0: surah index, 1: juz number]
   */
  virtual QPair<int, int> pageMetadata(const int page) const = 0;
  /**
   * @brief gets the page where the verse is found
   * @param surahIdx - sura number
   * @param verse - verse number
   * @return page number
   */
  virtual int getVersePage(const int& surahIdx, const int& verse) const = 0;
  /**
   * @brief gets the page where the corresponding juz starts
   * @param juz - juz number
   * @return page number
   */
  virtual Verse getJuzStart(const int juz) const = 0;
  /**
   * @brief get the juz which the passed page is a part of
   * @param page - page number
   * @return juz number
   */
  virtual int getVerseJuz(const Verse verse) const = 0;
  /**
   * @brief gets a QList of Verse instances for the page verses
   * @param page - Quran page number
   * @return QList of Verse instances
   */
  virtual QList<Verse> verseInfoList(const int page) const = 0;
  /**
   * @brief gets the verse text
   * @param sIdx - sura number (1-114)
   * @param vIdx - verse number
   * @return QString of the verse text
   */
  virtual QString verseText(const int sIdx, const int vIdx) const = 0;
  /**
   * @brief gets the page where the surah begins
   * @param surahIdx - sura number
   * @return page of the first verse in the sura
   */
  virtual int surahStartPage(int surahIdx) const = 0;
  /**
   * @brief gets the surah name in English or Arabic (default is English)
   * @param sIdx - sura number
   * @param ar - boolean to return arabic sura name
   * @return QString containing the sura name
   */
  virtual QString surahName(const int sIdx, bool ar = false) const = 0;
  /**
   * @brief get the verse with the corresponding id and return it as a Verse
   * instance
   * @param id - verse id
   * @return Verse instance
   */
  virtual Verse verseById(const int id) const = 0;
  /**
   * @brief gets the page where the verse is found
   * @param surahIdx - sura number
   * @param verse - verse number
   * @return page number
   */
  virtual int versePage(const int& surahIdx, const int& verse) const = 0;
  /**
   * @brief searches the database for surahs matching the given text pattern,
   * the pattern can be either in English or Arabic
   * @param text - name / part of the name of the sura
   * @return QList of sura numbers which contain the given text
   */
  virtual QList<int> searchSurahNames(QString text) const = 0;
  /**
   * @brief search specific surahs for the given search text
   * @param searchText - text to search for
   * @param surahs - QList of surah numbers to search in
   * @param whole - boolean value to search for whole words only
   * @return QList of Verse instances representing the search results
   */
  virtual QList<Verse> searchSurahs(QString searchText,
                                    const QList<int> surahs,
                                    const bool whole = false) const = 0;
  /**
   * @brief search a range of pages for the given search text
   * @param searchText - text to search for
   * @param range - array of start & end page numbers
   * @param whole - boolean value to indicate search for whole words only
   * @return QList of Verse instances representing the search results
   */
  virtual QList<Verse> searchVerses(QString searchText,
                                    const int range[2] = new int[2]{ 1, 604 },
                                    const bool whole = false) const = 0;
  /**
   * @brief gets a random verse from the Quran
   * @return QPair of Verse instance and verse text
   */
  virtual Verse randomVerse() const = 0;

  virtual Verse next(const Verse& verse, bool withBasmallah) const = 0;

  virtual Verse previous(const Verse& verse, bool withBasmallah) const = 0;
  /**
   * @brief surahNames
   * @return
   */
  virtual QStringList surahNames() const = 0;
};

#endif
