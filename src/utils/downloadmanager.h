/**
 * @file downloadmanager.h
 * @brief Header file for DownloadManager
 */

#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "../types/reciter.h"
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
  enum DownloadType
  {
    QCF,
    Recitation,
    File
  };
  /**
   * @brief DownloadTask struct represents a single verse file download task
   * @details downloads are separated into 3 different types
   * DownloadType::Recitation - MP3 recitation for a single verse - reciter
   * combination
   * DownloadType::QCF - QCF v2 font files
   * DownloadType::File - single file download, used for downloading tafsir and
   * translation DB files
   */
  struct DownloadTask
  {
    /**
     * @brief metainfo vector for storing information about the download task
     * @details data in the metainfo QList depends on the DownloadType
     * DownloadType::Recitation - { reciter, surah, verse }
     * DownloadType::QCF - { -1, -1, page }
     * DownloadType::File - { k, idx, bytes } where
     * k: kind of file (0 for tafsir, 1 for translation)
     * idx: index of the file in its corresponding global QList
     * bytes: is the current number of bytes downloaded (updated automatically)
     */
    QList<int> metainfo;
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
   * @brief process the download group queue by adding appropriate download
   * tasks according to the DownloadType and metainfo
   */
  void processDownloadQueue();
  /**
   * @brief enqueues an entry in the download group queue
   * @param type - DownloadType of group
   * @param info - additional info of the download required
   */
  void addToQueue(DownloadType type, QPair<int, int> info = { -1, -1 });
  /**
   * @brief overload to enqueue a Recitation entry in the download group queue
   * @param reciter - reciter index in Globals::recitersList
   * @param surah - surah number to download
   */
  void addToQueue(int reciter, int surah);
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
   * @brief Emitted when the currently active download group is completed
   */
  void downloadCompleted(DownloadType type, const QList<int>& metainfo);
  /**
   * @fn void downloadErrored(int, int)
   */
  void downloadErrored(DownloadType type, const QList<int>& metainfo);
  /**
   * @fn void filesFound(int, int)
   * @brief Emitted when the current surah verses are found in recitations
   * directory
   */
  void filesFound(DownloadType type, const QList<int>& metainfo);

private:
  QSharedPointer<const QDir> m_downloadsDir = DirManager::downloadsDir;
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  const QList<QSharedPointer<Reciter>>& m_recitersList = Reciter::reciters;
  const QList<QSharedPointer<Tafsir>>& m_tafasirList = Tafsir::tafasir;
  const QList<QSharedPointer<Translation>>& m_trList =
    Translation::translations;
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
   * @brief enqueues QCF font file tasks
   */
  void enqeueQCF();
  /**
   * @brief enqueues a File task based on the given info
   * @param info
   */
  void enqeueTask(QPair<int, int> info);
  /**
   * @brief create a DownloadTask and add it to the download Queue
   * @param reciterIdx - ::Globals::recitersList index for the reciter
   * @param surah - surah number
   * @param verse - verse number
   */
  void enqeueTask(int reciterIdx, int surah, int verse);

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
   * @brief the total count of files in the current group download / total bytes
   * if downloading a single file
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
   * @brief currently active DownloadType
   */
  DownloadType m_activeType = Recitation;
  /**
   * @brief download group queue, used for quickly adding downloads that will be
   * expanding into separate DownloadTask (s) when its processed
   */
  QQueue<QPair<DownloadType, QPair<int, int>>> m_downloadQueue;
  /**
   * @brief individual DownloadTask queue
   */
  QQueue<DownloadTask> m_taskQueue;
  /**
   * @brief QTime object to get the download start time, used in calculating
   * download speed
   */
  QTime m_downloadStart;
};

#endif // DOWNLOADMANAGER_H
