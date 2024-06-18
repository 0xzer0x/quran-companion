#ifndef BOOKMARKSREPOSITORY_H
#define BOOKMARKSREPOSITORY_H

#include <QDir>
#include <QList>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <notifiers/bookmarksnotifier.h>
#include <repository/dbconnection.h>
#include <repository/quranrepository.h>
#include <service/quranservice.h>
#include <types/verse.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class BookmarksRepository
 * @brief The BookmarksRepository class represents a connection to the bookmarks
 * db file used for storing user data (bookmarks,khatmah,thoughts)
 */
class BookmarksRepository
  : public DbConnection
  , QSqlDatabase
{
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static BookmarksRepository& getInstance();

  void open() override;

  Type type() override;

  bool saveActiveKhatmah(const Verse& verse);

  QList<int> getAllKhatmah() const;

  QString getKhatmahName(const int id) const;

  std::optional<Verse> loadVerse(const int khatmahId) const;

  int addKhatmah(const Verse& verse,
                 const QString name,
                 const int id = -1) const;

  bool editKhatmahName(const int khatmahId, QString newName);

  void removeKhatmah(const int id) const;

  QList<Verse> bookmarkedVerses(int surahIdx = -1) const;

  bool isBookmarked(const Verse& verse) const;

  bool addBookmark(const Verse& verse);

  bool removeBookmark(const Verse& verse);

  void saveThoughts(Verse& verse, const QString& text);

  QString getThoughts(const Verse& verse) const;

  QList<QPair<Verse, QString>> allThoughts() const;

  void setActiveKhatmah(const int id);

  int activeKhatmah() const;

  const BookmarksNotifier* notifier() const;

private:
  BookmarksRepository();
  /**
   * @brief reference to the singleton QuranRepository instance
   */
  const QuranService* m_quranService;
  /**
   * @brief reference to the singleton Configuration instance
   */
  const Configuration& m_config;
  /**
   * @brief reference to the app configuration directory
   */
  const QDir& m_configDir;
  /**
   * @brief integer id of the current active khatmah
   */
  int m_activeKhatmah = 0;
};

#endif // BOOKMARKSREPOSITORY_H
