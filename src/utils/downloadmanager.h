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
   * @brief VerseTask struct represents a single verse file download task
   * @details Quran surahs are downloaded as separate verse mp3 files which are
   * represented as VerseTask instances
   */
  struct VerseTask
  {
    int surah{ -1 };      ///< surah number
    int verse{ -1 };      ///< verse number
    int reciterIdx{ -1 }; ///<  ::Globals::recitersList index for the reciter
    QUrl link{};          ///< download link for the verse
    QFileInfo
      downloadPath{}; ///< QFileInfo representing the path to save the file to
    QNetworkReply* networkReply = nullptr; ///< reply data of the web request
    /**
     * @brief resets the download task
     */
    void clear()
    {
      surah = verse = reciterIdx = -1;
      link.clear();
      networkReply = nullptr;
    }
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
   * @return VerseTask
   */
  VerseTask currentTask() const;
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
  void processDownloadQueue();
  /**
   * @brief process the surah queue which contains surahs to be downloaded, add
   * DownloadTask instance for each verse of the processed surah
   */
  void processSurahQueue();
  /**
   * @brief add a QPair of reciter & surah to the surah download queue
   * @param reciter - ::Globals::recitersList index for the reciter
   * @param surah - surah number to download
   */
  void addSurahToQueue(int reciter, int surah);
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
   * @brief Emitted when the download queue began processing
   */
  void downloadStarted();
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
   * @fn void downloadCanceled()
   */
  void downloadCanceled();
  /**
   * @fn void downloadComplete(int, int)
   * @brief Emitted when all surah download tasks are completed
   */
  void downloadComplete(int reciterIdx, int surah);
  /**
   * @fn void downloadError(int, int)
   */
  void downloadError(int reciterIdx, int surah);
  /**
   * @fn void surahFound(int, int)
   * @brief Emitted when the current surah verses are found in
   * Globals::recitationsDir
   */
  void surahFound(int reciterIdx, int surah);

private:
  const QDir& m_toplevelDownloadPath = Globals::recitationsDir;
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
   */
  int m_currSurahCount;
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
   * @brief the currently active VerseTask
   */
  VerseTask m_currentTask;
  /**
   * @brief surah download queue
   */
  QQueue<QPair<int, int>> m_surahQueue;
  /**
   * @brief individual verses download queue
   */
  QQueue<VerseTask> m_downloadQueue;
  /**
   * @brief QTime object to get the download start time, used in calculating
   * download speed
   */
  QTime m_downloadStart;
};

#endif // DOWNLOADMANAGER_H
