
#include "notificationmanager.h"

NotificationManager::NotificationManager(QObject* parent, DBManager* dbPtr)
  : QObject{ parent }
  , m_dbMgr{ dbPtr }
  , m_dtNow{ QDateTime::currentDateTime() }
  , m_sysTray{ new QSystemTrayIcon(this) }
  , m_trayMenu{ new QMenu() }
{

  addActions();
  m_sysTray->setContextMenu(m_trayMenu);
  m_sysTray->setIcon(QIcon(":/images/tray.png"));
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
NotificationManager::checkDailyVerse()
{

  QDateTime lastTimestamp;
  QFile timestamp = QDir::currentPath() + QDir::separator() + "ts";
  if (!timestamp.exists()) {
    if (!timestamp.open(QIODevice::WriteOnly)) {
      qWarning() << "Couldn't open timestamp file for daily notification check";
      return;
    }
    showVerseOfTheDay();
    timestamp.write(m_dtNow.toString(Qt::DateFormat::ISODate).toLatin1());
    timestamp.write("\n");
    timestamp.write(QString(QString::number(m_votd.first.page) + ":" +
                            QString::number(m_votd.first.surah) + ":" +
                            QString::number(m_votd.first.number))
                      .toLatin1());
    timestamp.close();

  } else {
    if (!timestamp.open(QIODevice::ReadWrite)) {
      qWarning() << "Couldn't open timestamp file for daily notification check";
      return;
    }
    lastTimestamp =
      QDateTime::fromString(timestamp.readLine(), Qt::DateFormat::ISODate);
    if (lastTimestamp.daysTo(m_dtNow) > 0) {
      showVerseOfTheDay();
      timestamp.seek(0);
      timestamp.write(m_dtNow.toString(Qt::ISODate).toLatin1());
      timestamp.write("\n");
      timestamp.write(QString(QString::number(m_votd.first.page) + ":" +
                              QString::number(m_votd.first.surah) + ":" +
                              QString::number(m_votd.first.number))
                        .toLatin1());
    } else {
      QList<QByteArray> data = timestamp.readLine(15).split(':');

      m_votd.first =
        Verse{ data.at(0).toInt(), data.at(1).toInt(), data.at(2).toInt() };
      m_votd.second =
        m_dbMgr->getVerseText(m_votd.first.surah, m_votd.first.number);

      setVotdMsg();
    }
    timestamp.close();
  }
}
void
NotificationManager::showVerseOfTheDay()
{
  m_votd = m_dbMgr->randomVerse();
  setVotdMsg();

  emit showVOTDmessagebox(m_votd);
  m_votdShown = true;
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
          &NotificationManager::togglePlayback,
          Qt::UniqueConnection);
  connect(show,
          &QAction::triggered,
          this,
          &NotificationManager::showWindow,
          Qt::UniqueConnection);
  connect(hide,
          &QAction::triggered,
          this,
          &NotificationManager::hideWindow,
          Qt::UniqueConnection);
  connect(prefs,
          &QAction::triggered,
          this,
          &NotificationManager::openPrefs,
          Qt::UniqueConnection);
  connect(exit,
          &QAction::triggered,
          this,
          &NotificationManager::exit,
          Qt::UniqueConnection);
  connect(about,
          &QAction::triggered,
          this,
          &NotificationManager::openAbout,
          Qt::UniqueConnection);
  connect(update,
          &QAction::triggered,
          this,
          &NotificationManager::checkForUpdates,
          Qt::UniqueConnection);
}

void
NotificationManager::setVotdMsg()
{
  QString msg;

  m_votd.second.truncate(m_votd.second.size() - 2);
  msg.append("«" + m_votd.second + "»");
  msg.append("<br>");
  msg.append(m_dbMgr->getTranslation(m_votd.first.surah, m_votd.first.number));
  msg.append("<br>");
  msg.append(tr("Surah: ") + m_dbMgr->getSurahName(m_votd.first.surah) + " - " +
             tr("Verse: ") + QString::number(m_votd.first.number));

  m_votd.second = msg;
}

QPair<Verse, QString>
NotificationManager::votd()
{
  if (!m_votdShown)
    checkDailyVerse();

  return m_votd;
}
