
#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "dbmanager.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

typedef DBManager::Verse Verse;

class NotificationManager : public QObject
{
  Q_OBJECT

public:
  explicit NotificationManager(QObject* parent = nullptr,
                               DBManager* dbPtr = nullptr);
  void notify(QString title, QString msg);
  void checkDailyVerse();
  void showVerseOfTheDay();

signals:
  void exit(int ret = 0);
  void togglePlayback();
  void checkForUpdates();
  void showWindow();
  void hideWindow();
  void openWebsite();
  void showVOTDmessagebox(Verse v, QString msg);

private:
  QDateTime m_dtNow;
  DBManager* m_dbMgr;
  QMenu* m_trayMenu;
  QSystemTrayIcon* m_sysTray;
};

#endif // NOTIFICATIONMANAGER_H
