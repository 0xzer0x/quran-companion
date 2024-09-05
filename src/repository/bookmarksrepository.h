#ifndef BOOKMARKSREPOSITORY_H
#define BOOKMARKSREPOSITORY_H

#include <QDir>
#include <QList>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <notifiers/bookmarksnotifier.h>
#include <repository/dbconnection.h>
#include <service/quranservice.h>
#include <types/verse.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class BookmarksRepository
 * @brief Manages a connection to the bookmarks database for storing user data
 * such as bookmarks, khatmah, and thoughts.
 *
 * This class is a singleton that handles database operations related to
 * bookmarks, khatmah, and thoughts. It extends `DbConnection` and
 * `QSqlDatabase` to manage database connections and queries.
 */
class BookmarksRepository
  : public DbConnection
  , private QSqlDatabase
{
public:
  /**
   * @brief Provides access to the singleton instance of BookmarksRepository.
   * @return Reference to the static class instance.
   */
  static BookmarksRepository& getInstance();
  /**
   * @brief Opens the connection to the bookmarks database and initializes the
   * database schema.
   */
  void open() override;
  /**
   * @brief Returns the type of database connection.
   * @return Type of the database connection as DbConnection::Bookmarks.
   */
  Type type() override;
  /**
   * @brief Saves the current position in khatmah.
   * @param verse The verse representing the position to save.
   * @return true if the operation was successful, false otherwise.
   */
  bool saveActiveKhatmah(const Verse& verse);
  /**
   * @brief Retrieves all khatmah IDs.
   * @return List of khatmah IDs.
   */
  QList<int> getAllKhatmah() const;
  /**
   * @brief Retrieves the name of a specific khatmah.
   * @param id The ID of the khatmah.
   * @return The name of the khatmah.
   */
  QString getKhatmahName(const int id) const;
  /**
   * @brief Loads the verse associated with a specific khatmah ID.
   * @param khatmahId The ID of the khatmah.
   * @return An optional Verse object representing the loaded verse, or
   * std::nullopt if not found.
   */
  std::optional<Verse> loadVerse(const int khatmahId) const;
  /**
   * @brief Adds a new khatmah or updates an existing one.
   * @param verse The verse associated with the khatmah.
   * @param name The name of the khatmah.
   * @param id The ID of the khatmah (-1 for a new khatmah).
   * @return The ID of the newly added or updated khatmah.
   */
  int addKhatmah(const Verse& verse,
                 const QString name,
                 const int id = -1) const;
  /**
   * @brief Edits the name of an existing khatmah.
   * @param khatmahId The ID of the khatmah to edit.
   * @param newName The new name for the khatmah.
   * @return true if the operation was successful, false otherwise.
   */
  bool editKhatmahName(const int khatmahId, QString newName);
  /**
   * @brief Removes a specific khatmah.
   * @param id The ID of the khatmah to remove.
   */
  void removeKhatmah(const int id) const;
  /**
   * @brief Retrieves a list of bookmarked verses.
   * @param surahIdx Optional filter for verses in a specific surah (-1 for no
   * filter).
   * @return List of bookmarked verses.
   */
  QList<Verse> bookmarkedVerses(int surahIdx = -1) const;
  /**
   * @brief Checks if a specific verse is bookmarked.
   * @param verse The verse to check.
   * @return true if the verse is bookmarked, false otherwise.
   */
  bool isBookmarked(const Verse& verse) const;
  /**
   * @brief Adds a verse to bookmarks.
   * @param verse The verse to bookmark.
   * @return true if the operation was successful, false otherwise.
   */
  bool addBookmark(const Verse& verse);
  /**
   * @brief Removes a verse from bookmarks.
   * @param verse The verse to remove from bookmarks.
   * @return true if the operation was successful, false otherwise.
   */
  bool removeBookmark(const Verse& verse);
  /**
   * @brief Saves thoughts associated with a specific verse.
   * @param verse The verse to save thoughts for.
   * @param text The thoughts to save.
   */
  void saveThoughts(Verse& verse, const QString& text);
  /**
   * @brief Retrieves thoughts associated with a specific verse.
   * @param verse The verse to retrieve thoughts for.
   * @return The thoughts associated with the verse.
   */
  QString getThoughts(const Verse& verse) const;
  /**
   * @brief Retrieves all thoughts stored in the database.
   * @return List of pairs containing verses and their associated thoughts.
   */
  QList<QPair<Verse, QString>> allThoughts() const;
  /**
   * @brief Sets the currently active khatmah ID.
   * @param id The ID of the khatmah to set as active.
   */
  void setActiveKhatmah(const int id);
  /**
   * @brief Retrieves the currently active khatmah ID.
   * @return The ID of the active khatmah.
   */
  int activeKhatmah() const;
  /**
   * @brief Provides access to the notifier for bookmark-related events.
   * @return Pointer to the BookmarksNotifier.
   */
  const BookmarksNotifier* notifier() const;

private:
  BookmarksRepository();
  /**
   * @brief Reference to the QuranService instance.
   */
  const QuranService* m_quranService;
  /**
   * @brief Reference to the singleton Configuration instance.
   */
  const Configuration& m_config;
  /**
   * @brief Reference to the application configuration directory.
   */
  const QDir& m_configDir;
  /**
   * @brief Integer ID of the current active khatmah.
   */
  int m_activeKhatmah = 0;
};

#endif // BOOKMARKSREPOSITORY_H
