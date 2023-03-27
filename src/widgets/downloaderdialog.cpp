#include "downloaderdialog.h"
#include "ui_downloaderdialog.h"

/*!
 * \brief DownloaderDialog::DownloaderDialog class constructor
 * \param parent pointer to parent widget
 * \param settingsptr pointer to settings instance to access app settings
 * \param downloader pointer to backend downloader object
 * \param dbMan pointer to database management/interaction object
 */
DownloaderDialog::DownloaderDialog(QWidget *parent,
                                   QSettings *settingsptr,
                                   DownloadManager *downloader,
                                   DBManager *dbMan)
    : QDialog(parent)
    , ui(new Ui::DownloaderDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Download Manager"));
    setWindowIcon(QIcon(":/assets/images/download-manager.png"));

    m_appSettings = settingsptr;
    m_downloaderPtr = downloader;
    m_dbPtr = dbMan;

    // treeview setup
    QStringList headers;
    headers.append(tr("Number"));
    headers.append(tr("Name"));
    m_treeModel.setHorizontalHeaderLabels(headers);
    ui->treeView->setModel(&m_treeModel);
    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    fillTreeView();

    if (m_appSettings->value("Theme").toInt() == 0) {
        m_ssProgBar = "QProgressBar {text-align: center;color:black;}";
    } else {
        m_ssProgBar = "QProgressBar {text-align: center;color:white;}";
    }

    // connectors
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
}

/*!
 * \brief DownloaderDialog::fillTreeView fill the treeView model with the reciters info
 */
void DownloaderDialog::fillTreeView()
{
    bool en = m_appSettings->value("Language").toString() == "العربية" ? false : true;

    for (Reciter &reciter : m_downloaderPtr->recitersList()) {
        QStandardItem *item = new QStandardItem(reciter.displayName);

        m_treeModel.invisibleRootItem()->appendRow(item);

        for (int j = 1; j <= 114; j++) {
            QString sName = m_dbPtr->getSurahName(j, en);
            QStandardItem *suraItem = new QStandardItem(sName);

            QList<QStandardItem *> rw;
            rw.append(new QStandardItem(QString::number(j)));
            rw.append(suraItem);

            item->appendRow(rw);
        }
    }
}

/*!
 * \brief DownloaderDialog::addToQueue adds selected surahs to the download queue
 */
void DownloaderDialog::addToQueue()
{
    QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();

    foreach (QModelIndex i, selected) {
        if (i.parent().row() < 0)
            continue;

        addTaskProgress(i.parent().row(), i.row() + 1);

        for (int j = 1; j <= m_dbPtr->getSurahVerseCount(i.row() + 1); j++) {
            m_downloaderPtr->enqeueVerseTask(i.parent().row(), i.row() + 1, j);
        }
    }

    setCurrentBar();

    m_downloaderPtr->processQueueHead();
}

/*!
 * \brief DownloaderDialog::addTaskProgress adds a download progress bar to the downloader dialog to indicate download state
 * \param reciterIdx index for the chosen reciter as in the treeView & in the Reciters list
 * \param surah integer value represents the surah number to download (1-114)
 */
void DownloaderDialog::addTaskProgress(int reciterIdx, int surah)
{
    QString reciter = m_downloaderPtr->recitersList().at(reciterIdx).displayName;
    QString surahName = m_treeModel.index(surah - 1, 1, m_treeModel.index(1, 0)).data().toString();

    QString objName = reciter + tr(" // Surah: ") + surahName;

    QFrame *prgFrm = new QFrame(ui->scrollAreaWidgetContents);
    prgFrm->setLayout(new QVBoxLayout);

    prgFrm->setObjectName(objName);

    QLabel *lbTitle = new QLabel;

    lbTitle->setText(prgFrm->objectName());

    prgFrm->layout()->addWidget(lbTitle);

    DownloadProgressBar *dpb = new DownloadProgressBar(prgFrm, m_dbPtr->getSurahVerseCount(surah));

    prgFrm->layout()->addWidget(dpb);

    m_frameLst.append(prgFrm);

    ui->lytFrameView->addWidget(prgFrm);
}

/*!
 * \brief DownloaderDialog::setCurrentBar sets the currently active download task progress bar in order to update displayed info
 */
void DownloaderDialog::setCurrentBar()
{
    if (m_frameLst.empty())
        return;

    m_currentLb = m_frameLst.at(0)->findChild<QLabel *>();
    m_currentLb->setText(tr("Downloading: ") + m_currentLb->parent()->objectName());

    m_currentBar = m_frameLst.at(0)->findChild<DownloadProgressBar *>();

    m_currentBar->setStyleSheet(m_ssProgBar);

    connect(m_downloaderPtr,
            &DownloadManager::downloadProgressed,
            m_currentBar,
            &DownloadProgressBar::updateProgress,
            Qt::UniqueConnection);
}

/*!
 * \brief DownloaderDialog::surahDownloaded slot to delete the finished progress bar on download completion
 */
void DownloaderDialog::surahDownloaded()
{
    delete m_frameLst.at(0);
    m_frameLst.pop_front();
    setCurrentBar();
}

/*!
 * \brief DownloaderDialog::downloadAborted slot to delete all download tasks / progress bars from dialog
 */
void DownloaderDialog::downloadAborted()
{
    qDeleteAll(m_frameLst);
    m_frameLst.clear();
}

/*!
 * \brief DownloaderDialog::topTaskDownloadError slot to update the current task in case of download error
 */
void DownloaderDialog::topTaskDownloadError()
{
    m_currentBar->setStyleSheet("QProgressBar {text-align: center;} QProgressBar::chunk "
                                "{border-radius:2px;background-color: red;}");

    m_currentLb->setText(tr("Couldn't download: ") + m_currentLb->parent()->objectName());
}

void DownloaderDialog::closeEvent(QCloseEvent *event)
{
    this->hide();
}

DownloaderDialog::~DownloaderDialog()
{
    delete ui;
}
