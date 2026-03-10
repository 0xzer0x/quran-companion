#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QPointer>
#include <QProcess>
#include <notifiers/notificationsender.h>
#include <notifiers/updatenotifier.h>

class VersionChecker : public QObject
{
  Q_OBJECT
public:
  explicit VersionChecker(QObject* parent = nullptr);
  NotificationSender* notifier();

public slots:
  void checkUpdates();

private slots:
  void handleReply(QPointer<QNetworkReply> reply);

private:
  void getLatestVersion();
  QProcess m_runner;
  QNetworkRequest m_versionReq;
  QNetworkAccessManager m_netMgr;
  UpdateNotifier m_notifier;
};

#endif // VERSIONCHECKER_H
