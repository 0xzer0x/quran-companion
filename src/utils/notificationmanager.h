
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
    explicit NotificationManager(QObject *parent = nullptr, DBManager *dbPtr = nullptr);
    void notify(QString title, QString msg);
    void setTooltip(QString text);
    void checkDailyVerse();
    void showVerseOfTheDay();
    QString votdStringEntry() const;

    QPair<Verse, QString> votd();

signals:
  void exit(int ret = 0);
  void togglePlayback();
  void checkForUpdates();
  void showWindow();
  void hideWindow();
  void openPrefs();
  void openAbout();
  void showVOTDmessagebox(QPair<Verse, QString> votd);

private:
  void addActions();
  void setVotdMsg();
  bool m_votdShown = false;
  QDateTime m_dtNow;
  DBManager *m_dbMgr;
  QMenu *m_trayMenu;
  QSystemTrayIcon *m_sysTray;
  QPair<Verse, QString> m_votd;
};

#endif // NOTIFICATIONMANAGER_H
