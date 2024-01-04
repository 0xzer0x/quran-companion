/**
 * @file shortcuthandler.h
 * @brief Header file for ShortcutHandler
 */

#ifndef SHORTCUTHANDLER_H
#define SHORTCUTHANDLER_H

#include "../globals.h"
#include <QKeySequence>
#include <QMap>
#include <QObject>
#include <QShortcut>
#include <QString>

/**
 * @brief ShortcutHandler class is responsible for setting up different
 * application shortcuts, handling shortcut changes, and emitting appropriate
 * signal according to the activated shortcut
 */
class ShortcutHandler : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget that will recieve the shortcut
   * events
   */
  explicit ShortcutHandler(QObject* parent = nullptr);

public slots:
  /**
   * @brief slot to reload the key sequence for the shortcut with the given key
   * @param key - QString of the shortcut key in the settings file
   */
  void shortcutChanged(QString key);

signals:
  void toggleReaderView();
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
  void zoomIn();
  void zoomOut();
  void incrementVolume();
  void decrementVolume();
  void bookmarkCurrent();
  void openBookmarks();
  void openKhatmah();
  void openDownloads();
  void openSearch();
  void openSettings();
  void openTafsir();
  void openAdvancedCopy();

private:
  const QSettings* m_settings = Globals::settings;
  const QMap<QString, QString>& m_shortcutsDescription =
    Globals::shortcutDescription;
  /**
   * @brief connect different QShortcut signals to their
   * corresponding signal in ShortcutHandler
   */
  void setupConnections();
  /**
   * @brief Hash table of application shortcuts accessible through their
   * settings name
   */
  QHash<QString, QShortcut*> m_shortcuts;
};

#endif // SHORTCUTHANDLER_H
