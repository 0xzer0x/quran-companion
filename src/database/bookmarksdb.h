#ifndef BOOKMARKSDB_H
#define BOOKMARKSDB_H

#include <QDir>
#include <QList>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <database/qurandb.h>
#include <interfaces/dbconnection.h>
#include <notifiers/bookmarksnotifier.h>
#include <types/verse.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

class BookmarksDb
  : public DbConnection
  , QSqlDatabase
{
public:
  static BookmarksDb& getInstance();
  void open();
  Type type();
  /**
   * @brief sets the given ::Verse as the last position reached in the current
   * active khatmah
   * @param v - ::Verse reached in khatmah
   */
  bool saveActiveKhatmah(const Verse& verse);
  /**
   * @brief get all available khatmah ids
   * @return QList of khatmah id(s)
   */
  QList<int> getAllKhatmah() const;
  /**
   * @brief get the name of the khatmah with id given
   * @return QString containing the khatmah name
   */
  QString getKhatmahName(const int id) const;
  /**
   * @brief gets the last position saved for the khatmah with the id given and
   * stores the position in the ::Verse v
   * @return boolean indicating a successful operation (false in case of error
   * and in case id does not exist)
   */
  bool loadVerse(const int khatmahId, Verse& verse) const;
  /**
   * @brief add a new khatmah/replace khatmah with given id with position of
   * ::Verse v
   * @param v - ::Verse to set as the khatmah position
   * @param name - new khatmah name
   * @param id - id of khatmah to replace, -1 means do not replace (default: -1)
   * @return id of newly added khatmah or id parameter if defined
   */
  int addKhatmah(const Verse& verse,
                 const QString name,
                 const int id = -1) const;
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
  void removeKhatmah(const int id) const;
  /**
   * @brief gets a QList of ::Verse instances representing the bookmarked verse
   * within the given sura (default gets all)
   * @param surahIdx - sura number (-1 returns all bookmarks)
   * @return QList of bookmarked verses
   */
  QList<Verse> bookmarkedVerses(int surahIdx = -1) const;
  /**
   * @brief checks whether the given ::Verse is bookmarked
   * @param vInfo - ::Verse instance to check
   * @return boolean
   */
  bool isBookmarked(const Verse& verse) const;
  /**
   * @brief add the given ::Verse to bookmarks
   * @param vInfo - ::Verse instance to add
   * @return boolean
   */
  bool addBookmark(const Verse& verse, bool silent);
  /**
   * @brief remove the given ::Verse from bookmarks
   * @param vInfo - ::Verse instance to remove
   * @return boolean indicating successful removal
   */
  bool removeBookmark(const Verse& verse, bool silent);
  /**
   * MODIFIED
   */
  void saveThoughts(Verse& verse, const QString& text);
  /**
   * MODIFIED
   */
  QString getThoughts(const Verse& verse) const;
  /**
   * @brief allThoughts
   * @return
   */
  QList<QPair<Verse, QString>> allThoughts() const;
  /**
   * @brief setter for m_activeKhatmah
   * @param id - id of the active khatmah
   */
  void setActiveKhatmah(const int id);
  int activeKhatmah() const;
  const BookmarksNotifier* notifier() const;

private:
  BookmarksDb();
  const QuranDb& m_quranDb;
  const Configuration& m_config;
  const QDir& m_configDir;
  BookmarksNotifier m_notifier;
  /**
   * @brief integer id of the current active khatmah
   */
  int m_activeKhatmah = 0;
};

#endif // BOOKMARKSDB_H
