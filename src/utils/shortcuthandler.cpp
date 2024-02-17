/**
 * @file shortcuthandler.cpp
 * @brief Implementation file for ShortcutHandler
 */

#include "shortcuthandler.h"
#include "../utils/settings.h"
#include <QApplication>
#include <QFile>
#include <QXmlStreamReader>

QMap<QString, QString> ShortcutHandler::shortcutsDescription;

void
ShortcutHandler::populateDescriptionMap()
{
  QFile shortcuts(":/resources/shortcuts.xml");
  if (!shortcuts.open(QIODevice::ReadOnly))
    qCritical("Couldn't Open Shortcuts XML");

  Settings::settings->beginGroup("Shortcuts");
  QXmlStreamReader reader(&shortcuts);
  while (!reader.atEnd() && !reader.hasError()) {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if (reader.name().toString() == "shortcut") {
        QString key = reader.attributes().value("key").toString();
        QString defBind = reader.attributes().value("default").toString();
        QString desc =
          qApp->translate("SettingsDialog",
                          reader.attributes().value("description").toLatin1());

        shortcutsDescription.insert(key, desc);
        if (!Settings::settings->contains(key))
          Settings::settings->setValue(key, defBind);
      }
    }
  }

  Settings::settings->endGroup();
  shortcuts.close();
}

QSharedPointer<ShortcutHandler>
ShortcutHandler::current()
{
  static QSharedPointer<ShortcutHandler> handler =
    QSharedPointer<ShortcutHandler>::create();
  return handler;
}

ShortcutHandler::ShortcutHandler(QObject* parent)
  : QObject(parent)
{
}

void
ShortcutHandler::createShortcuts(QObject* context)
{
  foreach (const QString& key, shortcutsDescription.keys()) {
    QKeySequence seq = qvariant_cast<QKeySequence>(
      Settings::settings->value("Shortcuts/" + key));
    m_shortcuts.insert(key, new QShortcut(seq, context));
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
  connect(m_shortcuts.value("TogglePlayerControls"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::togglePlayerControls);
  connect(m_shortcuts.value("ToggleReaderView"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::toggleReaderView);
  connect(m_shortcuts.value("ToggleMenubar"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::toggleMenubar);
  connect(m_shortcuts.value("ToggleNavDock"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::toggleNavDock);
  connect(m_shortcuts.value("TogglePlayback"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::togglePlayback);
  connect(m_shortcuts.value("VolumeUp"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::incrementVolume);
  connect(m_shortcuts.value("VolumeDown"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::decrementVolume);
  connect(m_shortcuts.value("BookmarkCurrent"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::bookmarkCurrent);
  connect(m_shortcuts.value("NextPage"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::nextPage);
  connect(m_shortcuts.value("PrevPage"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::prevPage);
  connect(m_shortcuts.value("NextVerse"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::nextVerse);
  connect(m_shortcuts.value("PrevVerse"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::prevVerse);
  connect(m_shortcuts.value("NextJuz"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::nextJuz);
  connect(m_shortcuts.value("PrevJuz"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::prevJuz);
  connect(m_shortcuts.value("NextSurah"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::nextSurah);
  connect(m_shortcuts.value("PrevSurah"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::prevSurah);
  connect(m_shortcuts.value("ZoomIn"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::zoomIn);
  connect(m_shortcuts.value("ZoomOut"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::zoomOut);
  connect(m_shortcuts.value("DownloaderDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openDownloads);
  connect(m_shortcuts.value("BookmarksDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openBookmarks);
  connect(m_shortcuts.value("KhatmahDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openKhatmah);
  connect(m_shortcuts.value("SearchDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openSearch);
  connect(m_shortcuts.value("SettingsDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openSettings);
  connect(m_shortcuts.value("ContentDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openTafsir);
  connect(m_shortcuts.value("CopyDialog"),
          &QShortcut::activated,
          this,
          &ShortcutHandler::openAdvancedCopy);
}

void
ShortcutHandler::shortcutChanged(QString key)
{
  m_shortcuts.value(key)->setKey(
    qvariant_cast<QKeySequence>(Settings::settings->value("Shortcuts/" + key)));
}
