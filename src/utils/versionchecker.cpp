#include "versionchecker.h"
#include <QApplication>
#include <QDir>
#include <QMessageBox>

VersionChecker::VersionChecker(QObject* parent)
  : QObject(parent)
  , m_notifier(this)
  , m_updateTool(QApplication::applicationDirPath() + QDir::separator() +
                 "QCMaintenanceTool.exe")
{
  m_versionReq.setUrl(QUrl::fromEncoded(
    "https://raw.githubusercontent.com/0xzer0x/quran-companion/main/VERSION"));
  m_versionReq.setTransferTimeout(1500);

  connect(
    &m_runner, &QProcess::finished, this, &VersionChecker::handleToolOutput);
  connect(&m_netMgr,
          &QNetworkAccessManager::finished,
          this,
          &VersionChecker::handleReply);
}

void
VersionChecker::checkUpdates()
{
  if (toolExists()) {
    m_runner.setWorkingDirectory(QApplication::applicationDirPath());
    m_runner.start(m_updateTool, QStringList("ch"));
    return;
  }
  getLatestVersion();
}

void
VersionChecker::handleToolOutput()
{
  QString output = m_runner.readAll();
  QString displayText;

  if (output.contains("There are currently no updates available.")) {
    displayText = tr("There are currently no updates available.");
    QMessageBox::information(nullptr, tr("Update info"), displayText);
  } else {
    displayText = tr("Updates available, do you want to open the update tool?");
    QMessageBox::StandardButton btn =
      QMessageBox::question(nullptr, tr("Updates info"), displayText);
    if (btn == QMessageBox::Yes)
      m_runner.startDetached(m_updateTool);
  }
}

void
VersionChecker::handleReply(QPointer<QNetworkReply> reply)
{
  int status =
    reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  if (status != 200)
    return;

  QString version(reply->readAll().trimmed());
  if (qApp->applicationVersion() == version)
    m_notifier.notifyLatest();
  else
    m_notifier.notifyUpdate(version);
}

void
VersionChecker::getLatestVersion()
{
  QNetworkReply* reply = m_netMgr.get(m_versionReq);
  reply->ignoreSslErrors();
}

bool
VersionChecker::toolExists()
{
  return QFileInfo::exists(m_updateTool);
}

const UpdateNotifier*
VersionChecker::notifier() const
{
  return &m_notifier;
}
