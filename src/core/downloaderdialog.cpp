#include "downloaderdialog.h"
#include "ui_downloaderdialog.h"

/*!
 * \brief DownloaderDialog::DownloaderDialog class constructor
 * @param parent pointer to parent widget
 * @param settingsptr pointer to settings instance to access app settings
 * @param downloader pointer to backend downloader object
 * @param dbMan pointer to database management/interaction object
 * @param iconsPath path to current theme icons
 */
DownloaderDialog::DownloaderDialog(QWidget *parent,
                                   QSettings *settingsptr,
                                   DownloadManager *downloader,
                                   DBManager *dbMan,
                                   const QString &iconsPath)
    : QDialog(parent)
    , ui(new Ui::DownloaderDialog)
    , m_resourcePath{iconsPath}
    , m_appSettings{settingsptr}
    , m_downloaderPtr{downloader}
    , m_dbMgr{dbMan}
    , m_surahDisplayNames{m_dbMgr->surahNameList()}

{
    ui->setupUi(this);
    setWindowIcon(QIcon(m_resourcePath + "/icons/download-manager.png"));
    m_ssProgBar = "QProgressBar {text-align: center; color:palette(text); "
                  "border-radius: 2px; border: 1px "
                  "solid palette(dark); }";

    // treeview setup
    QStringList headers;
    headers.append(tr("Number"));
    headers.append(tr("Name"));
    m_treeModel.setHorizontalHeaderLabels(headers);
    ui->treeView->setModel(&m_treeModel);
    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    fillTreeView();

    // connectors
    setupConnections();
}

void DownloaderDialog::setupConnections()
{
    connect(ui->btnAddToQueue,
            &QPushButton::clicked,
            this,
            &DownloaderDialog::addToQueue,
            Qt::UniqueConnection);

    connect(ui->btnStopQueue,
            &QPushButton::clicked,
            m_downloaderPtr,
            &DownloadManager::stopQueue,
            Qt::UniqueConnection);

    connect(ui->btnClearQueue,
            &QPushButton::clicked,
            this,
            &DownloaderDialog::clearQueue,
            Qt::UniqueConnection);

    connect(m_downloaderPtr,
            &DownloadManager::downloadComplete,
            this,
            &DownloaderDialog::surahDownloaded,
            Qt::UniqueConnection);

    connect(m_downloaderPtr,
            &DownloadManager::downloadCanceled,
            this,
            &DownloaderDialog::downloadAborted,
            Qt::UniqueConnection);

    connect(m_downloaderPtr,
            &DownloadManager::downloadError,
            this,
            &DownloaderDialog::topTaskDownloadError,
            Qt::UniqueConnection);

    connect(m_downloaderPtr,
            &DownloadManager::downloadSpeedUpdated,
            this,
            &DownloaderDialog::updateDownloadSpeed);
}

/*!
 * \brief DownloaderDialog::fillTreeView fill the treeView model with the
 * reciters info
 */
void DownloaderDialog::fillTreeView()
{
    for (Reciter &reciter : m_downloaderPtr->recitersList()) {
        QStandardItem *item = new QStandardItem(reciter.displayName);

        m_treeModel.invisibleRootItem()->appendRow(item);

        for (int j = 1; j <= 114; j++) {
            QStandardItem *suraItem = new QStandardItem(m_surahDisplayNames.at(j - 1));

            QList<QStandardItem *> rw;
            rw.append(new QStandardItem(QString::number(j)));
            rw.append(suraItem);

            item->appendRow(rw);
        }
    }
}

void DownloaderDialog::addToDownloading(int reciter, int surah)
{
    // add surah to downloading tasks
    QSet<int> downloading = m_downloadingTasks.value(reciter);
    downloading.insert(surah);
    m_downloadingTasks.insert(reciter, downloading);
}

void DownloaderDialog::removeFromDownloading(int reciter, int surah)
{
    QSet<int> downloading = m_downloadingTasks.value(reciter);
    downloading.remove(surah);
    if (downloading.isEmpty())
        m_downloadingTasks.remove(reciter);
    else
        m_downloadingTasks.insert(reciter, downloading);
}

/*!
 * \brief DownloaderDialog::addToQueue adds selected surahs to the download
 * queue
 */
void DownloaderDialog::addToQueue()
{
    QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();

    int reciter = -1, surah = -1;
    foreach (QModelIndex i, selected) {
        reciter = i.parent().row();
        surah = i.row() + 1;
        bool currentlyDownloading = m_downloadingTasks.value(reciter).contains(surah);
        if (reciter < 0 || currentlyDownloading)
            continue;

        addToDownloading(reciter, surah);
        addTaskProgress(reciter, surah);
        for (int j = 1; j <= m_dbMgr->getSurahVerseCount(surah); j++) {
            m_downloaderPtr->enqeueVerseTask(reciter, surah, j);
        }
    }

    setCurrentBar();

    m_downloaderPtr->processQueueHead();
}

/*!
 * \brief DownloaderDialog::addTaskProgress adds a download progress bar to the
 * downloader dialog to indicate download state
 *
 * @param reciterIdx index for the chosen reciter as in the treeView & in the
 * Reciters list
 * @param surah integer value represents the surah number to download (1-114)
 */
void DownloaderDialog::addTaskProgress(int reciterIdx, int surah)
{
    QString reciter = m_downloaderPtr->recitersList().at(reciterIdx).displayName;
    QString surahName = m_surahDisplayNames.at(surah - 1);

    QString objName = reciter + tr(" // Surah: ") + surahName;

    QFrame *prgFrm = new QFrame(ui->scrollAreaWidgetContents);
    prgFrm->setLayout(new QVBoxLayout);
    prgFrm->setObjectName(objName);

    QBoxLayout *downInfo;
    if (m_appSettings->value("Language").toInt() == 14)
        downInfo = new QBoxLayout(QBoxLayout::RightToLeft, prgFrm);
    else
        downInfo = new QHBoxLayout(prgFrm);

    QLabel *lbTitle = new QLabel(prgFrm);
    lbTitle->setObjectName("DownloadInfo");
    lbTitle->setText(prgFrm->objectName());
    QLabel *downSpeed = new QLabel(prgFrm);
    downSpeed->setObjectName("DownloadSpeed");
    downSpeed->setAlignment(Qt::AlignRight);

    downInfo->addWidget(lbTitle);
    downInfo->addWidget(downSpeed);
    prgFrm->layout()->addItem(downInfo);

    DownloadProgressBar *dpb = new DownloadProgressBar(prgFrm, m_dbMgr->getSurahVerseCount(surah));
    prgFrm->layout()->addWidget(dpb);
    m_frameLst.append(prgFrm);

    ui->lytFrameView->addWidget(prgFrm);
}

/*!
 * \brief DownloaderDialog::setCurrentBar sets the currently active download
 * task progress bar in order to update displayed info
 */
void DownloaderDialog::setCurrentBar()
{
    if (m_frameLst.empty())
        return;

    m_currentLb = m_frameLst.at(0)->findChild<QLabel *>("DownloadInfo");
    m_currDownSpeedLb = m_frameLst.at(0)->findChild<QLabel *>("DownloadSpeed");
    m_currentLb->setText(tr("Downloading: ") + m_currentLb->parent()->objectName());

    m_currentBar = m_frameLst.at(0)->findChild<DownloadProgressBar *>();

    m_currentBar->setStyleSheet(m_ssProgBar);

    connect(m_downloaderPtr,
            &DownloadManager::downloadProgressed,
            m_currentBar,
            &DownloadProgressBar::updateProgress,
            Qt::UniqueConnection);
}

void DownloaderDialog::updateDownloadSpeed(int value, QString unit)
{
    m_currDownSpeedLb->setText(QString::number(value) + " " + unit + tr("/sec"));
}

void DownloaderDialog::selectTask(int reciter, int surah)
{
    QItemSelectionModel *selector = ui->treeView->selectionModel();
    QModelIndex reciterIdx = m_treeModel.index(reciter, 0);
    ui->treeView->collapseAll();
    ui->treeView->expand(reciterIdx);
    selector->clearSelection();
    selector->select(m_treeModel.index(surah - 1, 1, reciterIdx),
                     QItemSelectionModel::Rows | QItemSelectionModel::Select);
}

void DownloaderDialog::clearQueue()
{
    m_downloadingTasks.clear();
    m_downloaderPtr->stopQueue();
    if (!m_finishedFrames.isEmpty()) {
        qDeleteAll(m_finishedFrames);
        m_finishedFrames.clear();
    }
}

/*!
 * \brief DownloaderDialog::surahDownloaded slot to delete the finished progress
 * bar on download completion
 */
void DownloaderDialog::surahDownloaded(int reciter, int surah)
{
    m_currentBar->setStyling(DownloadProgressBar::completed);
    m_currentLb->setText(m_currentLb->parent()->objectName());
    m_currDownSpeedLb->setText(tr("Download Completed"));
    disconnect(m_downloaderPtr,
               &DownloadManager::downloadProgressed,
               m_currentBar,
               &DownloadProgressBar::updateProgress);

    removeFromDownloading(reciter, surah);
    m_finishedFrames.append(m_frameLst.front());
    m_frameLst.pop_front();
    setCurrentBar();
}

/*!
 * \brief DownloaderDialog::downloadAborted slot to delete all download tasks /
 * progress bars from dialog
 */
void DownloaderDialog::downloadAborted()
{
    m_downloadingTasks.clear();
    if (!m_frameLst.isEmpty()) {
        qDeleteAll(m_frameLst);
        m_frameLst.clear();
    }
}

/*!
 * \brief DownloaderDialog::topTaskDownloadError slot to update the current task
 * in case of download error
 */
void DownloaderDialog::topTaskDownloadError(int reciter, int surah)
{
    m_currentBar->setStyling(DownloadProgressBar::aborted);
    m_currentLb->setText(m_currentLb->parent()->objectName());
    m_currDownSpeedLb->setText(tr("Download Failed"));
    disconnect(m_downloaderPtr,
               &DownloadManager::downloadProgressed,
               m_currentBar,
               &DownloadProgressBar::updateProgress);

    removeFromDownloading(reciter, surah);
    m_finishedFrames.append(m_frameLst.front());
    m_frameLst.pop_front();
    setCurrentBar();
}

void DownloaderDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
}

DownloaderDialog::~DownloaderDialog()
{
    delete ui;
}
