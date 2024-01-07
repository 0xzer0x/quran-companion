/**
 * @file downloaderdialog.h
 * @brief Header file for DownloaderDialog
 */

#ifndef DOWNLOADERDIALOG_H
#define DOWNLOADERDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../utils/downloadmanager.h"
#include "../widgets/downloadprogressbar.h"
#include <QCloseEvent>
#include <QDesktopServices>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QShortcut>
#include <QStandardItem>
#include <QStandardItemModel>

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
   * @brief Class constructor
   * @param parent - pointer to parent widget
   * @param downloader - pointer to a DownloadManager instance
   * @param dbMan - pointer to DBManager instance
   */
  explicit DownloaderDialog(QWidget* parent = nullptr,
                            DownloadManager* downloader = nullptr);
  ~DownloaderDialog();

public slots:
  /**
   * @brief Adds the currently selected surahs in the QTreeView to the
   * DownloadManager download queue.
   * @details Adding surah to download queue is done through getting the surah
   * length through the database instance, then adding download tasks for every
   * verse in the surah selected.
   */
  void addToQueue();
  /**
   * @brief Sets the currently active download
   * task progress bar in order to update displayed info.
   * @details This is typically the first incomplete bar in the dialog.
   */
  void setCurrentBar();
  /**
   * @brief slot to delete the finished progress bar on download completion
   * @param reciter - ::Globals::recitersList index for the reciter whose
   * recitations are downloaded
   * @param surah - surah number
   */
  void surahDownloaded(DownloadType type, const int metainfo[]);
  /**
   * @brief slot to delete all download tasks /
   * progress bars from dialog
   */
  void downloadAborted();
  /**
   * @brief slot to update the current task
   * in case of download error
   * @param reciter - ::Globals::recitersList index for the reciter
   * @param surah - surah number
   */
  void topTaskDownloadError(DownloadType type, const int metainfo[]);
  /**
   * @brief slot to update the displayed download speed in the currently active
   * download.
   * @param value - download speed value
   * @param unit - unit which is being downloaded per second (KB, MB, etc.)
   */
  void updateDownloadSpeed(int value, QString unit);
  /**
   * @brief Selects a surah within a specific reciter in order to download.
   * @details Typically called when recitaion is missing in order to select the
   * missing surah automatically.
   * @param reciter - ::Globals::recitersList index for the reciter
   * @param surah - surah number
   */
  void selectTask(int reciter, int surah);
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
  const int m_languageCode = Globals::language;
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief Populates the QTreeView through which the user selects the surahs to
   * add to queue.
   */
  void addRecitationsToModel();
  /**
   * @brief Adds a download progress bar to the
   * downloader dialog to indicate download state
   * @param reciterIdx - ::Globals::recitersList index for the reciter whose
   * recitations are being downloaded
   * @param surah - number of surah being downloaded
   *
   * MODIFIED
   */
  void addTaskProgress(DownloadType type,
                       QPair<int, int> info = QPair<int, int>(-1, -1));
  /**
   * @brief enqueue a surah to download
   * @param reciter - ::Globals::recitersList index for the reciter whose
   * recitations are being downloaded
   * @param surah - number of surah to download
   */
  void enqueueSurah(int reciter, int surah);
  /**
   * @brief Adds the combination of reciter & surah to the currently active
   * downloads QHash.
   * @param reciter - ::Globals::recitersList index for the reciter whose
   * recitations are being downloaded
   * @param surah - surah number
   */
  void addToDownloading(int reciter, int surah);
  /**
   * @brief Removes the surah from the reciters currently active
   * downloads.
   * @param reciter - ::Globals::recitersList index for the reciter whose
   * recitations are being downloaded
   * @param surah - surah number
   */
  void removeFromDownloading(int reciter, int surah);
  /**
   * @brief Pointer to access ui elements generated from .ui files.
   */
  Ui::DownloaderDialog* ui;
  /**
   * @brief Pointer to the currently active DownloadProgressBar to update as
   * verses are downloaded.
   */
  DownloadProgressBar* m_currentBar;
  /**
   * @brief Pointer to DownloadManager instance.
   */
  DownloadManager* m_downloaderPtr;
  /**
   * @brief Pointer to QLabel which contains the state and information for the
   * currently active download.
   */
  QLabel* m_currentLb;
  /**
   * @brief Pointer to QLabel which contains the download speed/state.
   */
  QLabel* m_currDownSpeedLb;
  /**
   * @brief QFrames for the currently active & queued download tasks.
   */
  QList<QFrame*> m_frameLst;
  /**
   * @brief QFrames for the downloaded tasks.
   */
  QList<QFrame*> m_finishedFrames;
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
  QHash<int, QSet<int>> m_downloadingTasks;
};

#endif // DOWNLOADERDIALOG_H
