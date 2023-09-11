/**
 * @file notificationmanager.h
 * @brief Header file for NotificationManager
 */

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "../globals.h"
#include "dbmanager.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

/**
 * @brief NotificationManager class is responsible for system tray functionality
 * and verse of the day notification
 */
class NotificationManager : public QObject
{
  Q_OBJECT

public:
  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   */
  explicit NotificationManager(QObject* parent = nullptr);
  ~NotificationManager();

  /**
   * @brief send a desktop notification message
   * @param title - notification title
   * @param msg - notification message
   */
  void notify(QString title, QString msg);
  /**
   * @brief sets the tooltip for the system tray icon
   * @param text - tooltip message
   */
  void setTooltip(QString text);
  /**
   * @brief checks if the verse of the day has been set and set it if not found
   */
  void checkDailyVerse();
  /**
   * @brief get verse of the day and emit signal to show verse of the day
   * messagebox
   */
  void showVerseOfTheDay();
  /**
   * @brief serialize the verse of the day into a QString
   * @return QString of PAGE:SURA:NUMBER
   */
  QString votdStringEntry() const;
  /**
   * @brief getter for m_votd
   * @return QPair of the ::Verse of the day and the verse text
   */
  QPair<Verse, QString> votd();

signals:
  /**
   * @fn void exit(int)
   * @brief signal connected to the 'Exit' systray action
   * @param ret - exit code
   */
  void exit(int ret = 0);
  /**
   * @fn void togglePlayback()
   * @brief signal connected to 'toggle play/pause' systray action
   */
  void togglePlayback();
  /**
   * @fn void checkForUpdates()
   * @brief signal connected to 'check for updates' systray action
   */
  void checkForUpdates();
  /**
   * @fn void showWindow()
   * @brief signal connected to 'show window' systray action
   */
  void showWindow();
  /**
   * @fn void hideWindow()
   * @brief signal connected to 'hide window' systray action
   */
  void hideWindow();
  /**
   * @fn void openPrefs()
   * @brief signal connected to 'preferences' systray action
   */
  void openPrefs();
  /**
   * @fn void openAbout()
   * @brief signal connected to 'about' systray action
   */
  void openAbout();
  /**
   * @fn void showVOTDmessagebox(QPair<Verse, QString>)
   * @brief signal emitted when no votd is found for the current date
   * @param votd - the fetched votd
   */
  void showVOTDmessagebox(QPair<Verse, QString> votd);

private:
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  /**
   * @brief adds system tray actions and set their connections
   */
  void addActions();
  /**
   * @brief utility to convert the plain verse text in m_votd to html formatted
   * text to be displayed in the messagebox
   */
  void setVotdMsg();
  QFile m_timestampFile = Globals::configDir.absoluteFilePath("votd.log");
  /**
   * @brief boolean to indicate whether the verse of the day have been shown
   * before
   */
  bool m_votdShown = false;
  /**
   * @brief date and time at application launch
   */
  QDateTime m_dtNow;
  /**
   * @brief system tray context menu
   */
  QMenu* m_trayMenu;
  /**
   * @brief QSystemTrayIcon instance
   */
  QSystemTrayIcon* m_sysTray;
  /**
   * @brief QPair representing a random ::Verse and the verse text
   */
  QPair<Verse, QString> m_votd;
};

#endif // NOTIFICATIONMANAGER_H
