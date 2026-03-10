#include "versionchecker.h"
#include <QApplication>
#include <QDir>
#include <QMessageBox>

VersionChecker::VersionChecker(QObject* parent)
  : QObject(parent)
  , m_notifier(this)
{
  m_versionReq.setUrl(QUrl::fromEncoded(
    "https://raw.githubusercontent.com/0xzer0x/quran-companion/main/VERSION"));
  m_versionReq.setTransferTimeout(1500);

  connect(&m_netMgr,
          &QNetworkAccessManager::finished,
          this,
          &VersionChecker::handleReply);
}

void
VersionChecker::checkUpdates()
{
  getLatestVersion();
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

NotificationSender*
VersionChecker::notifier()
{
  return &m_notifier;
}
