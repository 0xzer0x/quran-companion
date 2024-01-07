/**
 * @file downloadmanager.h
 * @brief Header file for DownloadManager
 */

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

/**
 * @brief DownloadManager class is responsible for downloading files and
 * preforming web requests
 */
class DownloadManager : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief DownloadTask struct represents a single verse file download task
   * @details Quran surahs are downloaded as separate verse mp3 files which are
   * represented as DownloadTask instances
   *
   * MODIFIED
   */
  struct DownloadTask
  {
    /**
     * @brief metainfo array for storing information about the download task
     * @details in case of a regular verse download, { reciter, surah, verse }
     * and in case of QCF font download, { -1, -1, page }
     */
    int metainfo[3] = { -1, -1, -1 };
    /**
     * @brief download link for the verse
     */
    QUrl link;
    /**
     * @brief reply data of the web request
     */
    QNetworkReply* reply = nullptr;
    /**
     * @brief QFileInfo representing the path to save the file to
     */
    QFileInfo downloadPath;
  };

  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   */
  explicit DownloadManager(QObject* parent = nullptr);
  /**
   * @brief gets the latest release of the application from the github repo
   */
  void getLatestVersion();
  /**
   * @brief getter for m_isDownloading
   * @return boolean
   */
  bool isDownloading() const;
  /**
   * @brief getter for m_currentTask
   * @return DownloadTask
   */
  DownloadTask currentTask() const;
  /**
   * @brief getter for m_netMan
   * @return QNetworkAccessManager*
   */
  QNetworkAccessManager* netMan() const;

public slots:
  /**
   * @brief starts the download queue to process download tasks
   */
  void startQueue();
  /**
   * @brief stops the download process
   */
  void stopQueue();
  /**
   * @brief cancels the currently active download task
   */
  void cancelCurrentTask();
  /**
   * @brief process download queue front task. sets the networkReply for the
   * current task
   */
  void processTaskQueue();
  /**
   * @brief process the surah queue which contains surahs to be downloaded, add
   * DownloadTask instance for each verse of the processed surah
   *
   * MODIFIED
   */
  void processDownloadQueue();
  /**
   * @brief add a QPair of reciter & surah to the surah download queue
   * @param reciter - ::Globals::recitersList index for the reciter
   * @param surah - surah number to download
   */
  void addSurahToQueue(int reciter, int surah);
  /**
   * @brief addQCFToQueue
   *
   * MODIFIED
   */
  void addQCFToQueue();
  /**
   * @brief calculate download speed and emit signal for UI component to update
   * its value
   * @param bytes - bytes downloaded so far
   * @param total - total bytes
   */
  void downloadProgress(qint64 bytes, qint64 total);
  /**
   * @brief handle finished tasks and emit the correct signal
   * @param replyData - received data from request
   */
  void finishupTask(QNetworkReply* replyData);
  /**
   * @brief save downloaded verse file locally
   * @param data - downloaded binary data
   * @return boolean value to indicate successful write operation
   */
  bool saveFile(QNetworkReply* data);

signals:
  /**
   * @fn void latestVersionFound(QString)
   * @brief Emitted when the application version is fetched from github
   * @param appVer - application version string
   */
  void latestVersionFound(QString appVer);
  /**
   * @fn void downloadStarted()
   * @brief Emitted when the download queue begins processing
   */
  void downloadStarted();
  /**
   * @fn void downloadCanceled()
   */
  void downloadCanceled();
  /**
   * @fn void downloadProgressed(int, int)
   * @brief Emitted when a download task from the queue completed
   */
  void downloadProgressed(int downloaded, int total);
  /**
   * @fn void downloadSpeedUpdated(int, QString)
   * @brief Emitted to signal UI change for the displayed download speed
   */
  void downloadSpeedUpdated(int valuePerSec, QString unit);
  /**
   * @fn void downloadCompleted(int, int)
   * @brief Emitted when all surah download tasks are completed
   *
   * MODIFIED
   */
  void downloadCompleted(DownloadType type, const int metainfo[]);
  /**
   * @fn void downloadErrored(int, int)
   */
  void downloadErrored(DownloadType type, const int metainfo[]);
  /**
   * @fn void filesFound(int, int)
   * @brief Emitted when the current surah verses are found in recitations
   * directory
   */
  void filesFound(DownloadType type, const int metainfo[]);

private:
  const QDir& m_downloadsDir = Globals::downloadsDir;
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  /**
   * @brief generate download url for specified verse using the reciter download
   * url
   * @param reciterIdx - ::Globals::recitersList index for the reciter
   * @param surah - surah number
   * @param verse - verse number
   * @return
   */
  QUrl downloadUrl(const int reciterIdx,
                   const int surah,
                   const int verse) const;
  /**
   * @brief create a DownloadTask and add it to the download Queue
   * @param reciterIdx - ::Globals::recitersList index for the reciter
   * @param surah - surah number
   * @param verse - verse number
   */
  void enqeueVerseTask(int reciterIdx, int surah, int verse);
  void enqeueQCFTasks();
  /**
   * @brief emit signal according to the download error that occured
   * @param err - network error received
   */
  void handleConError(QNetworkReply::NetworkError err);
  /**
   * @brief emit signal with the latest application version
   */
  void handleVersionReply();
  /**
   * @brief boolean value representing the download state
   */
  bool m_isDownloading = false;
  /**
   * @brief the verse count of the current surah being downloaded
   *
   * MODIFIED
   */
  int m_activeTotal;
  QNetworkRequest m_versionReq;
  /**
   * @brief QNetworkReply for version info request
   */
  QNetworkReply* m_versionReply;
  /**
   * @brief QNetwrokAccessManager instance responsible for sending download
   * requests
   */
  QNetworkAccessManager* m_netMan;
  /**
   * @brief the currently active DownloadTask
   */
  DownloadTask m_activeTask;
  /**
   * @brief m_activeType
   *
   * MODIFIED
   */
  DownloadType m_activeType = Recitation;
  /**
   * @brief surah download queue
   *
   * MODIFIED
   */
  QQueue<QPair<DownloadType, QPair<int, int>>> m_downloadQueue;
  /**
   * @brief individual verses download queue
   *
   * MODIFIED
   */
  QQueue<DownloadTask> m_taskQueue;
  /**
   * @brief QTime object to get the download start time, used in calculating
   * download speed
   */
  QTime m_downloadStart;
};

#endif // DOWNLOADMANAGER_H
