#ifndef VERSIONCHECKER_H
#define VERSIONCHECKER_H

#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QPointer>
#include <QProcess>

class VersionChecker : public QObject
{
  Q_OBJECT
public:
  explicit VersionChecker(QObject* parent = nullptr);

public slots:
  void checkUpdates();

signals:
  void versionFound(QString version);

private slots:
  void handleToolOutput();
  void handleReply(QPointer<QNetworkReply> reply);

private:
  void getLatestVersion();
  bool toolExists();
  QNetworkAccessManager m_netMgr;
  QString m_updateTool;
  QProcess m_runner;
  QNetworkRequest m_versionReq;
};

#endif // VERSIONCHECKER_H
