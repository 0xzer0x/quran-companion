#ifndef BOOKMARKSDB_H
#define BOOKMARKSDB_H

#include <QDir>
#include <QList>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <database/qurandb.h>
#include <interfaces/dbconnection.h>
#include <utils/dirmanager.h>
#include <utils/settings.h>

class BookmarksDb
  : public DbConnection
  , QSqlDatabase
{
  Q_OBJECT
public:
  static QSharedPointer<BookmarksDb> current();
  BookmarksDb();
  void open();
  Type type();
  /**
   * @brief sets the given ::Verse as the last position reached in the current
   * active khatmah
   * @param v - ::Verse reached in khatmah
   */
  bool saveActiveKhatmah(QList<int> vInfo);
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
  bool loadVerse(const int khatmahId, QList<int>& vInfo);
  /**
   * @brief add a new khatmah/replace khatmah with given id with position of
   * ::Verse v
   * @param v - ::Verse to set as the khatmah position
   * @param name - new khatmah name
   * @param id - id of khatmah to replace, -1 means do not replace (default: -1)
   * @return id of newly added khatmah or id parameter if defined
   */
  int addKhatmah(QList<int> vInfo, const QString name, const int id = -1);
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
   * @brief gets a QList of ::Verse instances representing the bookmarked verse
   * within the given sura (default gets all)
   * @param surahIdx - sura number (-1 returns all bookmarks)
   * @return QList of bookmarked verses
   */
  QList<QList<int>> bookmarkedVerses(int surahIdx = -1);
  /**
   * @brief checks whether the given ::Verse is bookmarked
   * @param vInfo - ::Verse instance to check
   * @return boolean
   */
  bool isBookmarked(QList<int> vInfo);
  /**
   * @brief add the given ::Verse to bookmarks
   * @param vInfo - ::Verse instance to add
   * @return boolean
   */
  bool addBookmark(QList<int> vInfo);
  /**
   * @brief remove the given ::Verse from bookmarks
   * @param vInfo - ::Verse instance to remove
   * @return boolean indicating successful removal
   */
  bool removeBookmark(QList<int> vInfo);
  /**
   * @brief setter for m_activeKhatmah
   * @param id - id of the active khatmah
   */
  void setActiveKhatmah(const int id);
  /**
   * MODIFIED
   */
  void saveThoughts(QList<int> vInfo, const QString& text);
  /**
   * MODIFIED
   */
  QString getThoughts(QList<int> vInfo);

  int activeKhatmah() const;

signals:
  void bookmarkAdded();
  void bookmarkRemoved();

private:
  const QSharedPointer<QuranDb> m_quranDb = QuranDb::current();
  const QSharedPointer<QSettings> m_settings = Settings::settings;
  const QSharedPointer<QDir> m_configDir = DirManager::configDir;
  /**
   * @brief integer id of the current active khatmah
   */
  int m_activeKhatmah = 0;
};

#endif // BOOKMARKSDB_H
