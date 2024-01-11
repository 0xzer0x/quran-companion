/**
 * @file notificationmanager.cpp
 * @brief Implementation file for NotificationManager
 */

#include "notificationmanager.h"

NotificationManager::NotificationManager(QObject* parent)
  : QObject{ parent }
  , m_dtNow{ QDateTime::currentDateTime() }
  , m_sysTray{ new QSystemTrayIcon(this) }
  , m_trayMenu{ new QMenu() }
{
  addActions();
  m_sysTray->setContextMenu(m_trayMenu);
  m_sysTray->setIcon(QIcon(":/resources/tray.png"));
  m_sysTray->show();
}

void
NotificationManager::notify(QString title, QString msg)
{
  m_sysTray->showMessage(title, msg);
}

void
NotificationManager::setTooltip(QString text)
{
  m_sysTray->setToolTip(text);
}

void
NotificationManager::checkDailyVerse(bool startup)
{
  QDateTime lastTimestamp;
  if (!m_timestampFile.exists()) {
    if (!m_timestampFile.open(QIODevice::WriteOnly)) {
      qWarning()
        << "Couldn't open m_timestampFile file for daily notification check";
      return;
    }
    genVerseOfTheDay();
    writeTimestamp();
    if (startup)
      emit showVOTDmessagebox(m_votd);

  } else {
    if (!m_timestampFile.open(QIODevice::ReadWrite)) {
      qWarning()
        << "Couldn't open m_timestampFile file for daily notification check";
      return;
    }

    lastTimestamp =
      QDateTime::fromString(m_timestampFile.readLine().trimmed(), Qt::ISODate);
    if (lastTimestamp.daysTo(m_dtNow) > 0) {
      genVerseOfTheDay();
      writeTimestamp();
      if (startup)
        emit showVOTDmessagebox(m_votd);
    } else {
      readVerseOfTheDay();
    }
  }

  m_votdShown = true;
}

void
NotificationManager::genVerseOfTheDay()
{
  m_votd = m_dbMgr->randomVerse();
  setVotdMsg();
}

void
NotificationManager::readVerseOfTheDay()
{
  QList<QByteArray> data = m_timestampFile.readLine(15).split(':');
  m_votd.first =
    Verse{ data.at(0).toInt(), data.at(1).toInt(), data.at(2).toInt() };
  m_votd.second =
    m_dbMgr->getVerseText(m_votd.first.surah, m_votd.first.number);
  setVotdMsg();
}

void
NotificationManager::writeTimestamp()
{
  m_timestampFile.seek(0);
  m_timestampFile.write(m_dtNow.toString(Qt::ISODate).toLatin1());
  m_timestampFile.write("\n");
  m_timestampFile.write(votdStringEntry().toLatin1());
  m_timestampFile.close();
}

QString
NotificationManager::votdStringEntry() const
{
  QString entry =
    QString::number(m_votd.first.page).rightJustified(3, '0') + ":" +
    QString::number(m_votd.first.surah).rightJustified(3, '0') + ":" +
    QString::number(m_votd.first.number).rightJustified(3, '0');

  return entry;
}

void
NotificationManager::addActions()
{
  QAction* togglePlay = new QAction(tr("Play/Pause recitation"), m_trayMenu);
  QAction* show = new QAction(tr("Show window"), m_trayMenu);
  QAction* hide = new QAction(tr("Hide window"), m_trayMenu);
  QAction* prefs = new QAction(tr("Preferences"), m_trayMenu);
  QAction* update = new QAction(tr("Check for updates"), m_trayMenu);
  QAction* about = new QAction(tr("About"), m_trayMenu);
  QAction* exit = new QAction(tr("Exit"), m_trayMenu);
  m_trayMenu->addAction(togglePlay);
  m_trayMenu->addSeparator();
  m_trayMenu->addAction(show);
  m_trayMenu->addAction(hide);
  m_trayMenu->addSeparator();
  m_trayMenu->addAction(prefs);
  m_trayMenu->addSeparator();
  m_trayMenu->addAction(update);
  m_trayMenu->addAction(about);
  m_trayMenu->addSeparator();
  m_trayMenu->addAction(exit);

  connect(togglePlay,
          &QAction::triggered,
          this,
          &NotificationManager::togglePlayback);
  connect(show, &QAction::triggered, this, &NotificationManager::showWindow);
  connect(hide, &QAction::triggered, this, &NotificationManager::hideWindow);
  connect(prefs, &QAction::triggered, this, &NotificationManager::openPrefs);
  connect(exit, &QAction::triggered, this, &NotificationManager::exit);
  connect(about, &QAction::triggered, this, &NotificationManager::openAbout);
  connect(
    update, &QAction::triggered, this, &NotificationManager::checkForUpdates);
}

void
NotificationManager::setVotdMsg()
{
  QString msg;

  msg.append("<font size='5' face='kfgqpc_hafs_uthmanic _script'>");
  msg.append("« " + m_votd.second + " »");
  msg.append("</font><hr/>");
  msg.append(m_dbMgr->getTranslation(m_votd.first.surah, m_votd.first.number));
  msg.append("<p align='center'>");
  msg.append(tr("Surah: ") + m_dbMgr->getSurahName(m_votd.first.surah) + " - " +
             tr("Verse: ") + QString::number(m_votd.first.number));
  msg.append("</p>");

  m_votd.second = msg;
}

QPair<Verse, QString>
NotificationManager::votd()
{
  if (!m_votdShown)
    checkDailyVerse(false);

  return m_votd;
}

NotificationManager::~NotificationManager()
{
  delete m_trayMenu;
}
