#include "../headers/downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent, DBManager *dbptr, QStringList reciterDirs)
    : QObject{parent}
    , m_reciterDirNames{reciterDirs}
{
    m_netMan = new QNetworkAccessManager(this);
    m_dbPtr = dbptr;

    m_downloadPath.setPath(QApplication::applicationDirPath());
    if (!m_downloadPath.exists("audio"))
        m_downloadPath.mkdir("audio");

    m_downloadPath.cd("audio");

    m_reciterBaseUrls.append("https://everyayah.com/data/Husary_64kbps/");
    m_reciterBaseUrls.append(
        "https://github.com/0xzer0x/quran-companion/raw/main/audio/husary_qasr_64kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/Husary_Mujawwad_64kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/Abdul_Basit_Murattal_64kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/Abdul_Basit_Mujawwad_128kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/Minshawy_Murattal_128kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/Minshawy_Mujawwad_64kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/Alafasy_64kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/khalefa_al_tunaiji_64kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/Yasser_Ad-Dussary_128kbps/");
    m_reciterBaseUrls.append("https://everyayah.com/data/mahmoud_ali_al_banna_32kbps/");

    foreach (QString n, m_reciterDirNames) {
        if (!m_downloadPath.exists(n)) {
            m_downloadPath.mkdir(n);
        }
    }

    connect(m_netMan,
            &QNetworkAccessManager::finished,
            this,
            &DownloadManager::finishupTask,
            Qt::UniqueConnection);
}

void DownloadManager::startQueue()
{
    if (!m_isDownloading) {
        processQueueHead();
        emit downloadStarted();
    }
}

void DownloadManager::stopQueue()
{
    if (m_isDownloading) {
        cancelCurrentTask();
        m_isDownloading = false;
        m_downloadQueue.clear();
    }
}

void DownloadManager::cancelCurrentTask()
{
    m_currentTask.networkReply->abort();
    m_currentTask.networkReply->close();
    emit downloadCanceled();
}

void DownloadManager::enqeueVerseTask(int reciterIdx, int surah, int verse)
{
    QString url = m_reciterBaseUrls.at(reciterIdx);
    url.append(QString::number(surah).rightJustified(3, '0'));
    url.append(QString::number(verse).rightJustified(3, '0'));
    url.append(".mp3");

    QUrl dl = QUrl::fromEncoded(url.toLocal8Bit());

    DownloadTask t;
    t.surah = surah;
    t.verse = verse;
    t.reciterIdx = reciterIdx;
    t.link = dl;

    m_downloadQueue.enqueue(t);
}

void DownloadManager::processQueueHead()
{
    if (m_downloadQueue.empty()) {
        m_isDownloading = false;
        return;
    }

    qInfo() << "Processing head...";
    m_currentTask = m_downloadQueue.dequeue();
    qInfo() << "current task:" << m_currentTask.link;
    m_downloadPath.cd(m_reciterDirNames.at(m_currentTask.reciterIdx));

    while (m_downloadPath.exists(m_currentTask.link.fileName())) {
        emit downloadProgressed(m_currentTask.verse,
                                m_dbPtr->getSurahVerseCount(m_currentTask.surah));

        if (m_currentTask.verse == m_dbPtr->getSurahVerseCount(m_currentTask.surah)) {
            emit downloadComplete();
        }

        if (m_downloadQueue.empty()) {
            m_isDownloading = false;
            return;
        }

        m_currentTask = m_downloadQueue.dequeue();
    }

    m_isDownloading = true;
    QNetworkRequest req(m_currentTask.link);
    req.setMaximumRedirectsAllowed(20);

    qInfo() << "Making request...";
    m_currentTask.networkReply = m_netMan->get(req);
}

void DownloadManager::finishupTask(QNetworkReply *replyData)
{
    if (m_currentTask.networkReply->error() != QNetworkReply::NoError) {
        handleConError(m_currentTask.networkReply->error());
        return;
    }
    qInfo() << m_downloadPath;

    saveFile(replyData, m_currentTask.link.fileName());

    m_downloadPath.cdUp();

    emit downloadProgressed(m_currentTask.verse, m_dbPtr->getSurahVerseCount(m_currentTask.surah));

    if (m_currentTask.verse == m_dbPtr->getSurahVerseCount(m_currentTask.surah)) {
        emit downloadComplete();
    }

    if (!m_downloadQueue.isEmpty())
        processQueueHead();
}

bool DownloadManager::saveFile(QNetworkReply *data, QString filename)
{
    QFile localFile(m_downloadPath.filePath(filename));

    if (!localFile.open(QIODevice::WriteOnly)) {
        qWarning() << "[WARNING] Couldn't open file:" << filename;
        return false;
    }

    const QByteArray fdata = data->readAll();
    m_currentTask.networkReply->close();

    localFile.write(fdata);
    localFile.close();

    return true;
}

bool DownloadManager::isDownloading() const
{
    return m_isDownloading;
}

void DownloadManager::handleConError(QNetworkReply::NetworkError err)
{
    switch (err) {
    case QNetworkReply::OperationCanceledError:
        qInfo() << m_currentTask.networkReply->errorString();
        break;

    default:
        qInfo() << m_currentTask.networkReply->errorString();
        emit downloadError();
    }
}

QNetworkAccessManager *DownloadManager::netMan() const
{
    return m_netMan;
}

QList<QString> DownloadManager::reciterDirNames() const
{
    return m_reciterDirNames;
}

DownloadManager::DownloadTask DownloadManager::currentTask() const
{
    return m_currentTask;
}
