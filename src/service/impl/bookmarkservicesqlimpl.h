#ifndef BOOKMARKSERVICESQLIMPL_H
#define BOOKMARKSERVICESQLIMPL_H

#include <QObject>
#include <repository/bookmarksrepository.h>
#include <service/bookmarkservice.h>

class BookmarkServiceSqlImpl : public BookmarkService
{
private:
  /**
   * @brief notifer instance used for sending notifications
   */
  BookmarksNotifier m_notifier;
  BookmarksRepository& m_bookmarkRepository;

public:
  BookmarkServiceSqlImpl();

  QList<Verse> bookmarkedVerses(int surahIdx) const override;

  bool isBookmarked(const Verse& verse) const override;

  bool addBookmark(const Verse& verse, bool silent) override;

  bool removeBookmark(const Verse& verse, bool silent) override;

  NotificationSender* notifier() override;
};

#endif // BOOKMARKSERVICESQLIMPL_H
