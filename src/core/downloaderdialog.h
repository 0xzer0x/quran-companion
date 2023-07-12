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
    explicit DownloaderDialog(QWidget *parent = nullptr,
                              QSettings *settingsptr = nullptr,
                              DownloadManager *downloader = nullptr,
                              DBManager *dbMan = nullptr,
                              const QString &iconsPath = ":/resources/light/");
    void fillTreeView();
    ~DownloaderDialog();

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
    void closeEvent(QCloseEvent *event);

private:
    void setupConnections();
    void addToDownloading(int reciter, int surah);
    void removeFromDownloading(int reciter, int surah);
    Ui::DownloaderDialog *ui;
    QString m_resourcePath;
    DownloadProgressBar *m_currentBar;
    QLabel *m_currentLb;
    QLabel *m_currDownSpeedLb;
    QList<QFrame *> m_frameLst;
    QList<QFrame *> m_finishedFrames;
    QStandardItemModel m_treeModel;
    DownloadManager *m_downloaderPtr;
    DBManager *m_dbMgr;
    QSettings *m_appSettings;
    QStringList m_surahDisplayNames;
    QMap<int, QSet<int>> m_downloadingTasks;
};

#endif // DOWNLOADERDIALOG_H
