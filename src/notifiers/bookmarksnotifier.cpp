#include "bookmarksnotifier.h"
#include <QApplication>

BookmarksNotifier::BookmarksNotifier(QObject* parent)
{
  setParent(parent);
}

void
BookmarksNotifier::notifyAdded()
{
  QString msg = QCoreApplication::translate("BookmarksNotifier",
                                            "Verse added to bookmarks");
  emit notify(bookmarkAdd, msg);
}

void
BookmarksNotifier::notifyRemoved()
{
  QString msg = QCoreApplication::translate("BookmarksNotifier",
                                            "Verse removed from bookmarks");
  emit notify(bookmarkRemove, msg);
}
