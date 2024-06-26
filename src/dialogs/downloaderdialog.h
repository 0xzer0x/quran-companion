/**
 * @file downloaderdialog.h
 * @brief Header file for DownloaderDialog
 */

#ifndef DOWNLOADERDIALOG_H
#define DOWNLOADERDIALOG_H

#include <QCloseEvent>
#include <QDesktopServices>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QShortcut>
#include <QStandardItem>
#include <QStandardItemModel>
#include <downloader/impl/jobmanager.h>
#include <repository/quranrepository.h>
#include <service/quranservice.h>
#include <types/reciter.h>
#include <types/tafsir.h>
#include <types/translation.h>
#include <utils/configuration.h>
#include <widgets/downloadprogressbar.h>

namespace Ui {
class DownloaderDialog;
}

/**
 * @brief DownloaderDialog is an interface for downloading surah
 * recitation files for any reciter available.
 * @details Downloading recitations works through enqueuing download tasks for
 * DownloadManager instance to download. Filling the reciters QTreeView requires
 * access to the DBManager instance.
 */
class DownloaderDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief class contructor
   * @param parent - pointer to parent widget
   * @param manager - pointer to the JobManager instance used for managing
   * DownloadJob(s)
   */
  explicit DownloaderDialog(QWidget* parent = nullptr,
                            JobManager* manager = nullptr);
  ~DownloaderDialog();

public slots:
  /**
   * @brief Adds the currently selected jobs in the QTreeView to the
   * DownloadManager download queue.
   */
  void addToQueue();
  /**
   * @brief Sets the currently active download task progress bar in order to
   * update displayed info.
   * @details This is typically the first incomplete bar in the dialog.
   */
  void setCurrentBar();
  /**
   * @brief slot to delete all download tasks/progress bars from dialog
   */
  void downloadAborted();
  /**
   * @brief Update UI elements on download group completion
   * @param type - DownloadType of the download group
   * @param metainfo - QList of download group information
   */
  void downloadCompleted(QSharedPointer<DownloadJob> finished);
  /**
   * @brief Callback function to update UI elements when the current active
   * download group fails
   * @param type - DownloadType of the download group
   * @param metainfo - QList of download group information
   */
  void topTaskDownloadError(QSharedPointer<DownloadJob> failed);
  /**
   * @brief slot to update the displayed download speed in the currently active
   * download.
   * @param value - download speed value
   * @param unit - unit which is being downloaded per second (KB, MB, etc.)
   */
  void updateDownloadSpeed(int value, QString unit);
  /**
   * @brief automatically select a group/task to download
   * @param type - DownloadType of the download group to select
   * @param info - metainfo for the download task
   */
  void selectDownload(DownloadJob::Type type,
                      QPair<int, int> info = QPair<int, int>(0, 1));
  /**
   * @brief Stops downloading tasks and clears the downloads scrollarea
   * (including completed tasks)
   */
  void clearQueue();

private slots:
  /**
   * @brief Stops all downloading tasks.
   */
  void btnStopClicked();
  /**
   * @brief open Globals::recitationsDir in the default file url handler
   * (file manager)
   */
  void openDownloadsDir();

protected:
  /**
   * @brief Re-implementation of QWidget::closeEvent() in order to hide the
   * window instead of closing it.
   * @param event
   */
  void closeEvent(QCloseEvent* event);

private:
  Ui::DownloaderDialog* ui;
  const Configuration& m_config;
  const QuranService* m_quranService;
  const QList<Reciter>& m_reciters;
  const QList<Tafsir>& m_tafasir;
  const QList<Translation>& m_translations;
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief Populates the QTreeView through which the user selects the surahs to
   * add to queue.
   */
  void populateTreeModel();
  /**
   * @brief adds a QFrame of download task info and a progressbar
   * @param type - DownloadType of the download group to select
   * @param info - download metainfo QPair used by DownloadType::Recitation and
   * DownloadType::File
   */
  void addTaskProgress(QSharedPointer<DownloadJob> job);
  /**
   * @brief enqueue a surah to download
   * @param reciter - ::Reciter::reciters index for the reciter whose
   * recitations are being downloaded
   * @param surah - number of surah to download
   */
  void enqueueSurah(int reciter, int surah);
  /**
   * @brief Adds the combination of reciter & surah to the currently active
   * downloads QHash.
   * @param reciter - ::Reciter::reciters index for the reciter whose
   * recitations are being downloaded
   * @param surah - surah number
   */
  void addToDownloading(int reciter, int surah);
  /**
   * @brief Removes the surah from the reciters currently active
   * downloads.
   * @param reciter - ::Reciter::reciters index for the reciter whose
   * recitations are being downloaded
   * @param surah - surah number
   */
  void removeFromDownloading(int reciter, int surah);
  /**
   * @brief Pointer to the currently active DownloadProgressBar to update as
   * verses are downloaded.
   */
  QPointer<DownloadProgressBar> m_currentBar;
  QPointer<JobManager> m_jobMgr;
  /**
   * @brief Pointer to QLabel which contains the state and information for the
   * currently active download.
   */
  QPointer<QLabel> m_currentLb;
  /**
   * @brief Pointer to QLabel which contains the download speed/state.
   */
  QPointer<QLabel> m_currDownSpeedLb;
  /**
   * @brief QFrames for the currently active & queued download tasks.
   */
  QList<QPointer<QFrame>> m_frameLst;
  /**
   * @brief QFrames for the downloaded tasks.
   */
  QList<QPointer<QFrame>> m_finishedFrames;
  /**
   * @brief Model for the recitation download QTreeView.
   */
  QStandardItemModel m_treeModel;
  /**
   * @brief QStringList for the surah names which are displayed in the
   * QTreeView.
   */
  QStringList m_surahDisplayNames;
  /**
   * @brief QHash for the currently active & queued download tasks.
   * @details Keys in the QHash represent the reciters indices. values are a set
   * of surah numbers. When adding downloads, the reciter - surah combination is
   * checked against this QHash in order to determine whether its a duplicate or
   * not.
   */
  QHash<int, QSet<int>> m_downloadingSurahs;
};

#endif // DOWNLOADERDIALOG_H
