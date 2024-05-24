#ifndef QURANREPOSITORY_H
#define QURANREPOSITORY_H

#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <interfaces/dbconnection.h>
#include <types/verse.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class QuranRepository
 * @brief The QuranRepository class represents a connection to the quran db file
 */
class QuranRepository
  : public DbConnection
  , QSqlDatabase
{
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static QuranRepository& getInstance();

  void open() override;

  Type type() override;

  QPair<int, int> pageMetadata(const int page) const;

  int getVersePage(const int& surahIdx, const int& verse) const;

  Verse getJuzStart(const int juz) const;

  int getVerseJuz(const Verse verse) const;

  QList<Verse> verseInfoList(const int page) const;

  QString verseText(const int sIdx, const int vIdx) const;

  int surahStartPage(int surahIdx) const;

  QString surahName(const int sIdx, bool ar = false) const;

  Verse verseById(const int id) const;

  int versePage(const int& surahIdx, const int& verse) const;

  QList<int> searchSurahNames(QString text) const;

  QList<Verse> searchSurahs(QString searchText,
                            const QList<int> surahs,
                            const bool whole = false) const;

  QList<Verse> searchVerses(QString searchText,
                            const int range[2] = new int[2]{ 1, 604 },
                            const bool whole = false) const;

  Verse randomVerse() const;

  QStringList surahNames() const;

  Verse next(const Verse& verse, bool withBasmallah) const;

  Verse previous(const Verse& verse, bool withBasmallah) const;

private:
  QuranRepository();
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;
  /**
   * @brief reference to the app assets directory
   */
  const QDir& m_assetsDir;
  /**
   * @brief QList of sura names (Arabic if UI language is Arabic, Otherwise
   * English)
   */
  QStringList m_surahNames;
};

#endif // QURANREPOSITORY_H
