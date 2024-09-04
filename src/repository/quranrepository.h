#ifndef QURANREPOSITORY_H
#define QURANREPOSITORY_H

#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <repository/dbconnection.h>
#include <types/verse.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class QuranRepository
 * @brief The QuranRepository class represents a connection to the Quran
 * database file.
 *
 * This class provides various methods to query the Quranic text and metadata
 * from a SQLite database. It is designed to be a singleton and manages database
 * connections and queries related to the Quran.
 */
class QuranRepository
  : public DbConnection
  , QSqlDatabase
{
public:
  /**
   * @brief Get a reference to the singleton instance of QuranRepository.
   * @return Reference to the static class instance.
   */
  static QuranRepository& getInstance();
  /**
   * @brief Open the connection to the database.
   * This method sets up the database connection and opens the database file.
   */
  void open() override;
  /**
   * @brief Get the type of the database connection.
   * @return The type of the database connection (Quran).
   */
  Type type() override;
  /**
   * @brief Get metadata for a specific page.
   * @param page The page number to query.
   * @return A pair containing the surah index and jozz (division) for the page.
   */
  QPair<int, int> pageMetadata(const int page) const;
  /**
   * @brief Get the starting rub (section) in a specific page.
   * @param page The page number to query.
   * @return An optional pair containing the rub (section) index and hizb
   * (division) if found.
   */
  std::optional<QPair<int, int>> getRubStartingInPage(const int page) const;
  /**
   * @brief Get the page number for a specific verse.
   * @param surahIdx The surah index of the verse.
   * @param verse The verse number.
   * @return The page number containing the verse.
   */
  int getVersePage(const int& surahIdx, const int& verse) const;
  /**
   * @brief Get the starting verse for a specific jozz (division).
   * @param juz The jozz (division) number.
   * @return The starting verse of the specified jozz.
   */
  Verse getJuzStart(const int juz) const;
  /**
   * @brief Get the jozz (division) of a specific verse.
   * @param verse The verse to query.
   * @return The jozz (division) of the verse.
   */
  int getVerseJuz(const Verse verse) const;
  /**
   * @brief Get a list of verses on a specific page.
   * @param page The page number to query.
   * @return A list of verses on the specified page.
   */
  QList<Verse> verseInfoList(const int page) const;
  /**
   * @brief Get the first verse on a specific page.
   * @param page The page number to query.
   * @return The first verse on the specified page.
   */
  Verse firstInPage(int page) const;
  /**
   * @brief Get the text of a specific verse.
   * @param sIdx The surah index of the verse.
   * @param vIdx The verse number.
   * @return The text of the specified verse.
   */
  QString verseText(const int sIdx, const int vIdx) const;
  /**
   * @brief Get the starting page of a specific surah.
   * @param surahIdx The surah index.
   * @return The starting page of the specified surah.
   */
  int surahStartPage(int surahIdx) const;
  /**
   * @brief Get the name of a specific surah.
   * @param sIdx The surah index.
   * @param ar If true, return the Arabic name; otherwise, return the English
   * name.
   * @return The name of the specified surah.
   */
  QString surahName(const int sIdx, bool ar = false) const;
  /**
   * @brief Get verse details by verse ID.
   * @param id The ID of the verse.
   * @return The verse details associated with the specified ID.
   */
  Verse verseById(const int id) const;
  /**
   * @brief Get the page number for a specific verse by its surah and verse
   * number.
   * @param surahIdx The surah index of the verse.
   * @param verse The verse number.
   * @return The page number containing the verse.
   */
  int versePage(const int& surahIdx, const int& verse) const;
  /**
   * @brief Search for surah names that match a given text.
   * @param text The text to search for in surah names.
   * @return A list of surah indices that match the search text.
   */
  QList<int> searchSurahNames(QString text) const;
  /**
   * @brief Search for verses in specific surahs that match a given text.
   * @param searchText The text to search for in verses.
   * @param surahs A list of surah indices to search within.
   * @param whole If true, search for whole words; otherwise, search for partial
   * matches.
   * @return A list of verses that match the search text.
   */
  QList<Verse> searchSurahs(QString searchText,
                            const QList<int> surahs,
                            const bool whole = false) const;
  /**
   * @brief Search for verses within a specified page range that match a given
   * text.
   * @param searchText The text to search for in verses.
   * @param range The page range to search within.
   * @param whole If true, search for whole words; otherwise, search for partial
   * matches.
   * @return A list of verses that match the search text.
   */
  QList<Verse> searchVerses(QString searchText,
                            const int range[2] = new int[2]{ 1, 604 },
                            const bool whole = false) const;
  /**
   * @brief Get a random verse from the Quran.
   * @return A randomly selected verse.
   */
  Verse randomVerse() const;
  /**
   * @brief Get a list of all surah names.
   * @return A list of surah names based on the configured language.
   */
  QStringList surahNames() const;
  /**
   * @brief Get the next verse after a specified verse.
   * @param verse The current verse.
   * @param withBasmallah If true, include the Basmallah when moving to the next
   * verse.
   * @return The next verse.
   */
  Verse next(const Verse& verse, bool withBasmallah) const;
  /**
   * @brief Get the previous verse before a specified verse.
   * @param verse The current verse.
   * @param withBasmallah If true, include the Basmallah when moving to the
   * previous verse.
   * @return The previous verse.
   */
  Verse previous(const Verse& verse, bool withBasmallah) const;

private:
  /**
   * @brief Constructor for QuranRepository.
   * Initializes the database connection and sets up the surah names list.
   */
  QuranRepository();
  /**
   * @brief Execute a SQL query and handle errors.
   * @param query The SQL query to execute.
   * @param errMsg Error message to log if the query fails.
   * @return True if the query was successful, false otherwise.
   */
  bool executeQuery(QSqlQuery& query, QString errMsg) const;
  /**
   * @brief Reference to the singleton Configuration instance.
   */
  Configuration& m_config;

  /**
   * @brief Reference to the app assets directory.
   */
  const QDir& m_assetsDir;

  /**
   * @brief QList of surah names (Arabic if UI language is Arabic, otherwise
   * English).
   */
  QStringList m_surahNames;
};

#endif // QURANREPOSITORY_H
