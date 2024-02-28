#include "bookmarksnotifier.h"

BookmarksNotifier::BookmarksNotifier(QObject* parent)
{
  setParent(parent);
}

void
BookmarksNotifier::notifyAdded()
{
  QString msg = tr("Verse added to bookmarks");
  emit notify(bookmarkAdd, msg);
}

void
BookmarksNotifier::notifyRemoved()
{
  QString msg = tr("Verse removed from bookmarks");
  emit notify(bookmarkRemove, msg);
}
