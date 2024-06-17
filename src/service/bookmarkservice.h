#ifndef BOOKMARKSERVICE_H
#define BOOKMARKSERVICE_H

#include <QList>
#include <QObject>
#include <interfaces/notificationsender.h>
#include <types/verse.h>

class BookmarkService : public QObject
{
public:
  /**
   * @brief gets a QList of Verse instances representing the bookmarked verse
   * within the given sura (default gets all)
   * @param surahIdx - sura number (-1 returns all bookmarks)
   * @return QList of bookmarked verses
   */
  virtual QList<Verse> bookmarkedVerses(int surahIdx = -1) const = 0;
  /**
   * @brief checks whether the given Verse is bookmarked
   * @param vInfo - Verse instance to check
   * @return boolean
   */
  virtual bool isBookmarked(const Verse& verse) const = 0;
  /**
   * @brief add the given Verse to bookmarks
   * @param vInfo - Verse instance to add
   * @return boolean
   */
  virtual bool addBookmark(const Verse& verse, bool silent) = 0;
  /**
   * @brief remove the given Verse from bookmarks
   * @param vInfo - Verse instance to remove
   * @return boolean indicating successful removal
   */
  virtual bool removeBookmark(const Verse& verse, bool silent) = 0;
  /**
   * @brief returns the address of the class notifier
   * @return - pointer to BookmarksNotifier instance
   */
  virtual NotificationSender* notifier() = 0;
};

#endif
