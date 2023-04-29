
#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include "dbmanager.h"

typedef DBManager::Verse Verse;

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    explicit NotificationManager(QObject *parent = nullptr, DBManager *dbPtr = nullptr);
    void checkDailyVerse();
    void showVerseOfTheDay();

signals:
    void exit(int ret = 0);
    void togglePlayback();
    void checkForUpdates();
    void showWindow();
    void hideWindow();

private:
    QDateTime m_dtNow;
    DBManager *m_dbMgr;
    QMenu *m_trayMenu;
    QSystemTrayIcon *m_sysTray;
};

#endif // NOTIFICATIONMANAGER_H
