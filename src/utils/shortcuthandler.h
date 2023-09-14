#ifndef SHORTCUTHANDLER_H
#define SHORTCUTHANDLER_H

#include "../globals.h"
#include <QKeySequence>
#include <QMap>
#include <QObject>
#include <QShortcut>
#include <QString>

class ShortcutHandler : public QObject
{
  Q_OBJECT
public:
  explicit ShortcutHandler(QObject* parent = nullptr);

public slots:
  /**
   * @brief slot to reload the key sequence for the shortcut with the given key
   * @param key - QString of the shortcut key in the settings file
   */
  void shortcutChanged(QString key);

signals:
  void toggleNavDock();
  void toggleMenubar();
  void togglePlayback();
  void nextPage();
  void prevPage();
  void nextVerse();
  void prevVerse();
  void nextJuz();
  void prevJuz();
  void nextSurah();
  void prevSurah();
  void incrementVolume();
  void decrementVolume();
  void bookmarkCurrent();
  void openBookmarks();
  void openDownloads();
  void openSearch();
  void openSettings();
  void openTafsir();

private:
  QSettings* const m_settings = Globals::settings;
  const QMap<QString, QString>& m_shortcutsDescription =
    Globals::shortcutDescription;
  /**
   * @brief connect different QShortcut activated() signals to their
   * corresponding signal in ShortcutHandler
   */
  void setupConnections();
  /**
   * @brief QMap of application shortcuts accessible through their settings
   * name
   */
  QMap<QString, QShortcut*> m_shortcuts;
};

#endif // SHORTCUTHANDLER_H
