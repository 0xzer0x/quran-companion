/**
 * @file notificationmanager.cpp
 * @brief Implementation file for SystemTray
 */

#include "systemtray.h"
#include <QApplication>

SystemTray::SystemTray(QObject* parent)
  : QObject(parent)
  , m_sysTray(new QSystemTrayIcon(this))
  , m_trayMenu(new QMenu())
{
  addActions();
  setTooltip(qApp->translate("MainWindow", "Quran Companion"));
  m_sysTray->setContextMenu(m_trayMenu);
  m_sysTray->setIcon(QIcon(":/resources/tray.png"));
  m_sysTray->show();
}

void
SystemTray::notify(QString title, QString msg)
{
  m_sysTray->showMessage(title, msg);
}

void
SystemTray::setTooltip(QString text)
{
  m_sysTray->setToolTip(text);
}

void
SystemTray::addActions()
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

  connect(togglePlay, &QAction::triggered, this, &SystemTray::togglePlayback);
  connect(show, &QAction::triggered, this, &SystemTray::showWindow);
  connect(hide, &QAction::triggered, this, &SystemTray::hideWindow);
  connect(prefs, &QAction::triggered, this, &SystemTray::openPrefs);
  connect(exit, &QAction::triggered, this, &SystemTray::exit);
  connect(about, &QAction::triggered, this, &SystemTray::openAbout);
  connect(update, &QAction::triggered, this, &SystemTray::checkForUpdates);
}

SystemTray::~SystemTray()
{
  delete m_trayMenu;
}
