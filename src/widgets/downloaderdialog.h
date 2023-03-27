#ifndef DOWNLOADERDIALOG_H
#define DOWNLOADERDIALOG_H

#include <QCloseEvent>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QSettings>
#include <QStandardItem>
#include <QStandardItemModel>
#include "../utils/dbmanager.h"
#include "../utils/downloadmanager.h"
#include "downloadprogressbar.h"

typedef DBManager::Reciter Reciter;

namespace Ui {
class DownloaderDialog;
}

/*!
 * \class DownloaderDialog
 * 
 * \brief The DownloaderDialog class is responsible for downloading surah recitation files for any reciter available 
 */
class DownloaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloaderDialog(QWidget *parent = nullptr,
                              QSettings *settingsptr = nullptr,
                              DownloadManager *downloader = nullptr,
                              DBManager *dbMan = nullptr);
    void fillTreeView();
    ~DownloaderDialog();

public slots:
    void addToQueue();
    void addTaskProgress(int reciterIdx, int surah);
    void setCurrentBar();
    void surahDownloaded();
    void downloadAborted();
    void topTaskDownloadError();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::DownloaderDialog *ui;
    DownloadProgressBar *m_currentBar;
    QLabel *m_currentLb;
    QList<QFrame *> m_frameLst;
    QStandardItemModel m_treeModel;
    DownloadManager *m_downloaderPtr;
    DBManager *m_dbPtr;
    QSettings *m_appSettings;
    QString m_ssProgBar;
};

#endif // DOWNLOADERDIALOG_H
