#include "bookmarkservicesqlimpl.h"

BookmarkServiceSqlImpl::BookmarkServiceSqlImpl()
  : m_bookmarkRepository(BookmarksRepository::getInstance())
  , m_notifier(this)
{
}

QList<Verse>
BookmarkServiceSqlImpl::bookmarkedVerses(int surahIdx) const
{
  return m_bookmarkRepository.bookmarkedVerses(surahIdx);
}

bool
BookmarkServiceSqlImpl::isBookmarked(const Verse& verse) const
{
  return m_bookmarkRepository.isBookmarked(verse);
}

bool
BookmarkServiceSqlImpl::addBookmark(const Verse& verse, bool silent)
{
  bool ok = m_bookmarkRepository.addBookmark(verse);
  if (ok && !silent)
    m_notifier.notifyAdded();
  return ok;
}

bool
BookmarkServiceSqlImpl::removeBookmark(const Verse& verse, bool silent)
{
  bool ok = m_bookmarkRepository.removeBookmark(verse);
  if (ok && !silent)
    m_notifier.notifyRemoved();
  return ok;
}

const BookmarksNotifier*
BookmarkServiceSqlImpl::notifier() const
{
  return &m_notifier;
}
