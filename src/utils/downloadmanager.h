#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

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

typedef DBManager::Reciter Reciter;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr,
                             DBManager *dbptr = nullptr,
                             QList<Reciter> reciters = QList<Reciter>());

    struct DownloadTask
    {
        int surah{-1};
        int verse{-1};
        int reciterIdx{-1};
        QUrl link{};
        QString filename{};
        QNetworkReply *networkReply = nullptr;
        void clear()
        {
            surah = verse = reciterIdx = -1;
            link.clear();
            networkReply = nullptr;
        }
    };

    bool isDownloading() const;
    DownloadTask currentTask() const;
    QNetworkAccessManager *netMan() const;
    QList<Reciter> recitersList() const;

public slots:
    void startQueue();
    void stopQueue();
    void cancelCurrentTask();
    void enqeueVerseTask(int reciterIdx, int surah, int verse);
    void processQueueHead();
    void downloadProgress(qint64 bytes, qint64 total);
    void finishupTask(QNetworkReply *replyData);
    bool saveFile(QNetworkReply *data);

signals:
    void downloadStarted();
    void downloadProgressed(int downloaded, int total);
    void downloadSpeedUpdated(int valuePerSec, QString unit);
    void downloadCanceled();
    void downloadComplete();
    void downloadError();
    void queueEmpty();

private:
    QUrl downloadUrl(const int reciterIdx, const int surah, const int verse) const;
    void handleConError(QNetworkReply::NetworkError err);
    bool m_isDownloading = false;
    int m_currSurahCount;
    QNetworkAccessManager *m_netMan;
    DBManager *m_dbMgr;
    DownloadTask m_currentTask;
    QQueue<DownloadTask> m_downloadQueue;
    const QList<Reciter> m_recitersList;
    QDir m_downloadPath;
    QDir m_topLevelPath;
    QTime m_downloadStart;
};

#endif // DOWNLOADMANAGER_H
