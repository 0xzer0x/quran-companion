/**
 * @file shortcuthandler.cpp
 * @brief Implementation file for ShortcutHandler
 */

#include "shortcuthandler.h"
#include <QApplication>
#include <QFile>
#include <QXmlStreamReader>
using std::make_pair;

ShortcutHandler&
ShortcutHandler::getInstance()
{
  static ShortcutHandler handler;
  return handler;
}

ShortcutHandler::ShortcutHandler()
  : m_config(Configuration::getInstance())
{
}

void
ShortcutHandler::populateDescriptionMap()
{
  QFile shortcuts(":/resources/shortcuts.xml");
  if (!shortcuts.open(QIODevice::ReadOnly))
    qCritical("Couldn't Open Shortcuts XML");

  m_config.settings().beginGroup("Shortcuts");
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

        m_shortcutsDescription.insert(key, desc);
        if (!m_config.settings().contains(key))
          m_config.settings().setValue(key, defBind);
      }
    }
  }

  m_config.settings().endGroup();
  shortcuts.close();
}

void
ShortcutHandler::createShortcuts(QObject* context)
{
  foreach (const QString& key, m_shortcutsDescription.keys()) {
    QKeySequence seq = qvariant_cast<QKeySequence>(
      m_config.settings().value("Shortcuts/" + key));
    m_shortcuts.insert(key, new QShortcut(seq, context));
  }
  m_shortcuts.value("TogglePlayback")->setContext(Qt::ApplicationShortcut);
  m_shortcuts.value("BookmarkCurrent")->setContext(Qt::ApplicationShortcut);

  setupConnections();
}

void
ShortcutHandler::setupConnections()
{
  connect(m_shortcuts.value("Quit"),
          &QShortcut::activated,
          qApp,
          &QApplication::quit);
  for (const auto& connection : {
         make_pair("TogglePlayerControls",
                   &ShortcutHandler::togglePlayerControls),
         make_pair("ToggleReaderView", &ShortcutHandler::toggleReaderView),
         make_pair("ToggleMenubar", &ShortcutHandler::toggleMenubar),
         make_pair("ToggleNavDock", &ShortcutHandler::toggleNavDock),
         make_pair("TogglePlayback", &ShortcutHandler::togglePlayback),
         make_pair("VolumeUp", &ShortcutHandler::incrementVolume),
         make_pair("VolumeDown", &ShortcutHandler::decrementVolume),
         make_pair("BookmarkCurrent", &ShortcutHandler::bookmarkCurrent),
         make_pair("NextPage", &ShortcutHandler::nextPage),
         make_pair("PrevPage", &ShortcutHandler::prevPage),
         make_pair("NextJuz", &ShortcutHandler::nextJuz),
         make_pair("PrevJuz", &ShortcutHandler::prevJuz),
         make_pair("NextSurah", &ShortcutHandler::nextSurah),
         make_pair("PrevSurah", &ShortcutHandler::prevSurah),
         make_pair("NextVerse", &ShortcutHandler::nextVerse),
         make_pair("PrevVerse", &ShortcutHandler::prevVerse),
         make_pair("ZoomIn", &ShortcutHandler::zoomIn),
         make_pair("ZoomOut", &ShortcutHandler::zoomOut),
         make_pair("DownloaderDialog", &ShortcutHandler::openDownloads),
         make_pair("BookmarksDialog", &ShortcutHandler::openBookmarks),
         make_pair("KhatmahDialog", &ShortcutHandler::openKhatmah),
         make_pair("SearchDialog", &ShortcutHandler::openSearch),
         make_pair("SettingsDialog", &ShortcutHandler::openSettings),
         make_pair("ContentDialog", &ShortcutHandler::openContent),
         make_pair("CopyDialog", &ShortcutHandler::openAdvancedCopy),
       }) {
    connect(m_shortcuts.value(connection.first),
            &QShortcut::activated,
            this,
            connection.second);
  }
}

void
ShortcutHandler::shortcutChanged(QString key)
{
  m_shortcuts.value(key)->setKey(qvariant_cast<QKeySequence>(
    m_config.settings().value("Shortcuts/" + key)));
}

const QMap<QString, QString> &ShortcutHandler::shortcutsDescription() const
{
  return m_shortcutsDescription;
}
