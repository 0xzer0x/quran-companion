/**
 * @file shortcuthandler.cpp
 * @brief Implementation file for ShortcutHandler
 */

#include "shortcuthandler.h"

ShortcutHandler::ShortcutHandler(QObject* parent)
  : QObject(parent)
{
  foreach (const QString& key, m_shortcutsDescription.keys()) {
    QKeySequence seq =
      qvariant_cast<QKeySequence>(m_settings->value("Shortcuts/" + key));
    m_shortcuts.insert(key, new QShortcut(seq, parent));
  }
  m_shortcuts.value("TogglePlayback")->setContext(Qt::ApplicationShortcut);
  m_shortcuts.value("BookmarkCurrent")->setContext(Qt::ApplicationShortcut);

  setupConnections();
}

void
ShortcutHandler::setupConnections()
{
  connect(m_shortcuts.value("Quit"), &QShortcut::activated, this, []() {
    emit QApplication::exit();
  });
  connect(m_shortcuts.value("ToggleReaderView"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::toggleReaderView,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("ToggleMenubar"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::toggleMenubar,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("ToggleNavDock"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::toggleNavDock,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("TogglePlayback"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::togglePlayback,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("VolumeUp"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::incrementVolume,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("VolumeDown"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::decrementVolume,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("BookmarkCurrent"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::bookmarkCurrent,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("NextPage"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::nextPage,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("PrevPage"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::prevPage,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("NextVerse"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::nextVerse,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("PrevVerse"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::prevVerse,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("NextJuz"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::nextJuz,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("PrevJuz"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::prevJuz,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("NextSurah"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::nextSurah,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("PrevSurah"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::prevSurah,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("ZoomIn"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::zoomIn,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("ZoomOut"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::zoomOut,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("DownloaderDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openDownloads,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("BookmarksDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openBookmarks,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("KhatmahDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openKhatmah,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("SearchDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openSearch,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("SettingsDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openSettings,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("TafsirDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openTafsir,
          Qt::UniqueConnection);
  connect(m_shortcuts.value("CopyDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openAdvancedCopy,
          Qt::UniqueConnection);
}

void
ShortcutHandler::shortcutChanged(QString key)
{
  m_shortcuts.value(key)->setKey(
    qvariant_cast<QKeySequence>(m_settings->value("Shortcuts/" + key)));
}
