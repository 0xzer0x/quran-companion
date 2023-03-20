#include "../../headers/interfaces/downloaderdialog.h"
#include "../ui_downloaderdialog.h"

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

    m_recitersList.append(tr("Al-Husary"));
    m_recitersList.append(tr("Al-Husary (Qasr)"));
    m_recitersList.append(tr("Al-Husary (Mujawwad)"));
    m_recitersList.append(tr("Abdul-Basit"));
    m_recitersList.append(tr("Abdul-Basit (Mujawwad)"));
    m_recitersList.append(tr("Menshawi"));
    m_recitersList.append(tr("Menshawi (Mujawwad)"));
    m_recitersList.append(tr("Mishary Alafasy"));
    m_recitersList.append(tr("Khalefa Al-tunaiji"));
    m_recitersList.append(tr("Yasser Ad-dussary"));
    m_recitersList.append(tr("Mahmoud Al-banna"));

    // treeview setup
    QStringList headers;
    headers.append(tr("Number"));
    headers.append(tr("Name"));
    m_treeModel.setHorizontalHeaderLabels(headers);
    ui->treeView->setModel(&m_treeModel);
    ui->treeView->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    fillTreeView();

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

void DownloaderDialog::fillTreeView()
{
    bool en = m_appSettings->value("Language").toString() == "العربية" ? false : true;

    for (QString &name : m_recitersList) {
        QStandardItem *item = new QStandardItem(name);

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

DownloaderDialog::~DownloaderDialog()
{
    delete ui;
}

void DownloaderDialog::addToQueue()
{
    QModelIndexList selected = ui->treeView->selectionModel()->selectedRows();

    foreach (QModelIndex i, selected) {
        qInfo() << "Parent row:" << i.parent().row() << "Surah row:" << i.row();

        if (i.parent().row() < 0)
            continue;

        addTaskProgress(i.parent().row(), i.row() + 1);

        for (int j = 1; j <= m_dbPtr->getSurahVerseCount(i.row() + 1); j++) {
            qInfo() << "Enqueuing reciter:" << i.parent().row() << "surah:" << i.row() + 1
                    << "ayah:" << j;

            m_downloaderPtr->enqeueVerseTask(i.parent().row(), i.row() + 1, j);
        }
    }

    setCurrentBar();

    m_downloaderPtr->processQueueHead();
}

void DownloaderDialog::addTaskProgress(int reciterIdx, int surah)
{
    QString reciter = m_recitersList.at(reciterIdx);
    QString surahName = m_treeModel.index(surah - 1, 1, m_treeModel.index(1, 0)).data().toString();

    QString objName = reciter + " // Surah: " + surahName;

    QFrame *prgFrm = new QFrame(ui->scrollAreaWidgetContents);
    prgFrm->setLayout(new QVBoxLayout);

    prgFrm->setObjectName(objName);

    qInfo() << prgFrm->objectName();
    QLabel *lbTitle = new QLabel;

    lbTitle->setText(prgFrm->objectName());

    prgFrm->layout()->addWidget(lbTitle);

    DownloadProgressBar *dpb = new DownloadProgressBar(prgFrm, m_dbPtr->getSurahVerseCount(surah));

    prgFrm->layout()->addWidget(dpb);

    m_frameLst.append(prgFrm);

    ui->lytFrameView->addWidget(prgFrm);
}

void DownloaderDialog::setCurrentBar()
{
    if (m_frameLst.empty())
        return;

    m_currentLb = m_frameLst.at(0)->findChild<QLabel *>();
    m_currentLb->setText(tr("Downloading: ") + m_currentLb->parent()->objectName());

    m_currentBar = m_frameLst.at(0)->findChild<DownloadProgressBar *>();
    connect(m_downloaderPtr,
            &DownloadManager::downloadProgressed,
            m_currentBar,
            &DownloadProgressBar::updateProgress,
            Qt::UniqueConnection);
}

void DownloaderDialog::surahDownloaded()
{
    delete m_frameLst.at(0);
    m_frameLst.pop_front();
    setCurrentBar();
}

void DownloaderDialog::downloadAborted()
{
    qDeleteAll(m_frameLst);
    m_frameLst.clear();
}

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
