/**
 * @file downloaderdialog.h
 * @brief Header file for DownloaderDialog, the main interface for managing and
 * monitoring Quran recitations, tafsir, and translation downloads.
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
 * @brief DownloaderDialog is the primary interface for managing the download of
 * Quran recitations, tafsir, and translation files.
 * @details This class interacts with a JobManager to manage download tasks and
 * with QuranRepository to fetch data for the available reciters, tafsirs, and
 * translations. The UI is updated dynamically based on download progress,
 * completion, or failure.
 */
class DownloaderDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief Constructor for DownloaderDialog.
   * @param parent Pointer to the parent widget.
   * @param manager Pointer to the JobManager instance used for managing
   * DownloadJob(s).
   */
  explicit DownloaderDialog(QWidget* parent = nullptr,
                            JobManager* manager = nullptr);
  ~DownloaderDialog();

public slots:
  /**
   * @brief Adds the currently selected jobs in the QTreeView to the download
   * queue.
   */
  void addToQueue();
  /**
   * @brief Updates the currently active download task's progress bar with the
   * latest progress.
   * @details Typically updates the first incomplete progress bar in the dialog.
   */
  void setCurrentBar();
  /**
   * @brief Deletes all download tasks and progress bars from the dialog.
   */
  void downloadAborted();
  /**
   * @brief Updates UI elements on download group completion.
   * @param finished Pointer to the completed DownloadJob.
   */
  void downloadCompleted(QSharedPointer<DownloadJob> finished);
  /**
   * @brief Updates UI elements when the current active download group fails.
   * @param failed Pointer to the failed DownloadJob.
   */
  void topTaskDownloadError(QSharedPointer<DownloadJob> failed);
  /**
   * @brief Updates the displayed download speed in the currently active
   * download.
   * @param value Download speed value.
   * @param unit Unit for the download speed (e.g., KB/s, MB/s).
   */
  void updateDownloadSpeed(int value, QString unit);
  /**
   * @brief Automatically selects a group/task to download.
   * @param type The type of download group to select.
   * @param info Metainfo for the download task, with a default value of
   * QPair<int, int>(0, 1).
   */
  void selectDownload(DownloadJob::Type type,
                      QPair<int, int> info = QPair<int, int>(0, 1));
  /**
   * @brief Stops downloading tasks and clears the downloads scroll area
   * (including completed tasks).
   */
  void clearQueue();

private slots:
  /**
   * @brief Stops all downloading tasks and resets the download UI elements.
   */
  void btnStopClicked();
  /**
   * @brief Opens the download directory (Globals::recitationsDir) in the
   * default file manager.
   */
  void openDownloadsDir();

protected:
  /**
   * @brief Re-implementation of QWidget::closeEvent() to hide the window
   * instead of closing it.
   * @param event Pointer to the QCloseEvent.
   */
  void closeEvent(QCloseEvent* event);

private:
  /**
   * @brief Connects signals and slots for different UI components and
   * shortcuts.
   */
  void setupConnections();
  /**
   * @brief Populates the QTreeView through which the user selects the surahs,
   * tafsir, or translation files to add to the download queue.
   */
  void populateTreeModel();
  /**
   * @brief Adds a QFrame containing download task information and a progress
   * bar.
   * @param job Pointer to the DownloadJob for which the progress bar is
   * created.
   */
  void addTaskProgress(QSharedPointer<DownloadJob> job);
  /**
   * @brief Enqueues a surah for download.
   * @param reciter Index of the reciter whose recitations are being downloaded.
   * @param surah Number of the surah to download.
   */
  void enqueueSurah(int reciter, int surah);
  /**
   * @brief Adds the combination of reciter and surah to the currently active
   * downloads.
   * @param reciter Index of the reciter whose recitations are being downloaded.
   * @param surah Surah number.
   */
  void addToDownloading(int reciter, int surah);
  /**
   * @brief Removes the surah from the reciter's currently active downloads.
   * @param reciter Index of the reciter whose recitations are being downloaded.
   * @param surah Surah number.
   */
  void removeFromDownloading(int reciter, int surah);
  /**
   * @brief Pointer to the dialog's UI.
   */
  Ui::DownloaderDialog* ui;
  /**
   * @brief Pointer to the currently active DownloadProgressBar to update as
   * files are downloaded.
   */
  QPointer<DownloadProgressBar> m_currentBar;
  /**
   * @brief Pointer to the JobManager instance responsible for managing download
   * jobs.
   */
  QPointer<JobManager> m_jobMgr;
  /**
   * @brief Pointer to QLabel containing the state and information for the
   * currently active download.
   */
  QPointer<QLabel> m_currentLb;
  /**
   * @brief Pointer to QLabel displaying the download speed and state.
   */
  QPointer<QLabel> m_currDownSpeedLb;
  /**
   * @brief List of QFrames for the currently active and queued download tasks.
   */
  QList<QPointer<QFrame>> m_frameLst;
  /**
   * @brief List of QFrames for the completed download tasks.
   */
  QList<QPointer<QFrame>> m_finishedFrames;
  /**
   * @brief Model for the download selection QTreeView.
   */
  QStandardItemModel m_treeModel;
  /**
   * @brief List of surah names displayed in the QTreeView.
   */
  QStringList m_surahDisplayNames;
  /**
   * @brief Hash of currently active and queued download tasks.
   * @details Keys in the QHash represent the reciter indices, and values are
   * sets of surah numbers. This QHash is used to check for duplicate
   * reciter-surah combinations when adding downloads.
   */
  QHash<int, QSet<int>> m_downloadingSurahs;
  /**
   * @brief Reference to the Configuration object providing application
   * settings.
   */
  const Configuration& m_config;
  /**
   * @brief Pointer to the QuranService instance providing Quran-related
   * services.
   */
  const QuranService* m_quranService;
  /**
   * @brief List of Reciter objects available for download.
   */
  const QList<Reciter>& m_reciters;
  /**
   * @brief List of Tafsir objects available for download.
   */
  const QList<Tafsir>& m_tafasir;
  /**
   * @brief List of Translation objects available for download.
   */
  const QList<Translation>& m_translations;
};

#endif // DOWNLOADERDIALOG_H
