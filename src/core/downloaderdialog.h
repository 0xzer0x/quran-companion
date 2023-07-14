#ifndef DOWNLOADERDIALOG_H
#define DOWNLOADERDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../utils/downloadmanager.h"
#include "../widgets/downloadprogressbar.h"
#include <QCloseEvent>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QStandardItem>
#include <QStandardItemModel>

namespace Ui {
class DownloaderDialog;
}

/*!
 * \class DownloaderDialog
 *
 * \brief The DownloaderDialog class is responsible for downloading surah
 * recitation files for any reciter available
 */
class DownloaderDialog : public QDialog
{
  Q_OBJECT

public:
  explicit DownloaderDialog(QWidget* parent = nullptr,
                            DownloadManager* downloader = nullptr,
                            DBManager* dbMan = nullptr);
  ~DownloaderDialog();

  void fillTreeView();

public slots:
  void addToQueue();
  void addTaskProgress(int reciterIdx, int surah);
  void setCurrentBar();
  void surahDownloaded(int reciter, int surah);
  void downloadAborted();
  void topTaskDownloadError(int reciter, int surah);
  void updateDownloadSpeed(int value, QString unit);
  void selectTask(int reciter, int surah);
  void clearQueue();

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);

private:
  const int m_languageCode = g_language;
  const QList<Reciter>& m_recitersList = g_recitersList;
  const QDir& m_resources = g_themeResources;
  void setupConnections();
  void addToDownloading(int reciter, int surah);
  void removeFromDownloading(int reciter, int surah);
  Ui::DownloaderDialog* ui;
  DownloadProgressBar* m_currentBar;
  DownloadManager* m_downloaderPtr;
  DBManager* m_dbMgr;
  QLabel* m_currentLb;
  QLabel* m_currDownSpeedLb;
  QList<QFrame*> m_frameLst;
  QList<QFrame*> m_finishedFrames;
  QStandardItemModel m_treeModel;
  QStringList m_surahDisplayNames;
  QMap<int, QSet<int>> m_downloadingTasks;
};

#endif // DOWNLOADERDIALOG_H
