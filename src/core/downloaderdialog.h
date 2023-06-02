#ifndef DOWNLOADERDIALOG_H
#define DOWNLOADERDIALOG_H

#include "../utils/dbmanager.h"
#include "../utils/downloadmanager.h"
#include "../widgets/downloadprogressbar.h"
#include <QCloseEvent>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QSettings>
#include <QStandardItem>
#include <QStandardItemModel>

typedef DBManager::Reciter Reciter;

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
                            QSettings* settingsptr = nullptr,
                            DownloadManager* downloader = nullptr,
                            DBManager* dbMan = nullptr,
                            const QString& iconsPath = ":/images/light/");
  void fillTreeView();
  ~DownloaderDialog();

public slots:
  void addToQueue();
  void addTaskProgress(int reciterIdx, int surah);
  void setCurrentBar();
  void surahDownloaded();
  void downloadAborted();
  void topTaskDownloadError();
  void updateDownloadSpeed(int value, QString unit);
  void selectTask(int reciter, int surah);
  void clearQueue();

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);

private:
  Ui::DownloaderDialog* ui;
  void setupConnections();
  QString m_iconsPath;
  DownloadProgressBar* m_currentBar;
  QLabel* m_currentLb;
  QLabel* m_currDownSpeedLb;
  QList<QFrame*> m_frameLst;
  QList<QFrame*> m_finishedFrames;
  QStandardItemModel m_treeModel;
  DownloadManager* m_downloaderPtr;
  DBManager* m_dbMgr;
  QSettings* m_appSettings;
  QStringList m_surahDisplayNames;
  QString m_ssProgBar =
    "QProgressBar {text-align: center; color:palette(text); "
    "border-radius: 2px; border: 1px "
    "solid palette(dark); }";
  ;
};

#endif // DOWNLOADERDIALOG_H
