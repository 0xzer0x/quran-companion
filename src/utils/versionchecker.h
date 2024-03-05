#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QPointer>
#include <QProcess>
#include <interfaces/notificationsender.h>
#include <notifiers/updatenotifier.h>

class VersionChecker : public QObject
{
  Q_OBJECT
public:
  explicit VersionChecker(QObject* parent = nullptr);
  const UpdateNotifier* notifier() const;

public slots:
  void checkUpdates();

private slots:
  void handleToolOutput();
  void handleReply(QPointer<QNetworkReply> reply);

private:
  bool toolExists();
  void getLatestVersion();
  QProcess m_runner;
  QString m_updateTool;
  QNetworkRequest m_versionReq;
  QNetworkAccessManager m_netMgr;
  UpdateNotifier m_notifier;
};

#endif // VERSIONCHECKER_H
