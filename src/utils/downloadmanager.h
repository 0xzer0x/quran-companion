#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "../globals.h"
#include "dbmanager.h"
#include <QApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QQueue>
#include <QTime>
#include <QUrl>

class DownloadManager : public QObject
{
  Q_OBJECT
public:
  struct DownloadTask
  {
    int surah{ -1 };
    int verse{ -1 };
    int reciterIdx{ -1 };
    QUrl link{};
    QString filename{};
    QNetworkReply* networkReply = nullptr;
    void clear()
    {
      surah = verse = reciterIdx = -1;
      link.clear();
      networkReply = nullptr;
    }
  };

  explicit DownloadManager(QObject* parent = nullptr,
                           DBManager* dbptr = nullptr);

  void getLatestVersion();
  bool isDownloading() const;
  DownloadTask currentTask() const;
  QNetworkAccessManager* netMan() const;

public slots:
  void startQueue();
  void stopQueue();
  void cancelCurrentTask();
  void enqeueVerseTask(int reciterIdx, int surah, int verse);
  void processQueueHead();
  void downloadProgress(qint64 bytes, qint64 total);
  void finishupTask(QNetworkReply* replyData);
  bool saveFile(QNetworkReply* data);

signals:
  void latestVersionFound(QString appVer);
  void downloadStarted();
  void downloadProgressed(int downloaded, int total);
  void downloadSpeedUpdated(int valuePerSec, QString unit);
  void downloadCanceled();
  void downloadComplete(int reciterIdx, int surah);
  void downloadError(int reciterIdx, int surah);
  void queueEmpty();

private:
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  QUrl downloadUrl(const int reciterIdx,
                   const int surah,
                   const int verse) const;
  void handleConError(QNetworkReply::NetworkError err);
  void handleVersionReply();
  bool m_isDownloading = false;
  int m_currSurahCount;
  QNetworkReply* m_versionReply;
  QNetworkAccessManager* m_netMan;
  DBManager* m_dbMgr;
  DownloadTask m_currentTask;
  QQueue<DownloadTask> m_downloadQueue;
  QDir m_downloadPath;
  QTime m_downloadStart;
};

#endif // DOWNLOADMANAGER_H
