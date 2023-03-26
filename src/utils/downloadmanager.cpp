#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent, DBManager *dbptr, QList<Reciter> reciters)
    : QObject(parent)
    , m_recitersList{reciters}
{
    m_netMan = new QNetworkAccessManager(this);
    m_dbPtr = dbptr;

    m_downloadPath.setPath(QApplication::applicationDirPath());
    if (!m_downloadPath.exists("audio"))
        m_downloadPath.mkdir("audio");

    m_downloadPath.cd("audio");
    foreach (Reciter r, m_recitersList) {
        if (!m_downloadPath.exists(r.baseDirName)) {
            m_downloadPath.mkdir(r.baseDirName);
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
    QString url = m_recitersList.at(reciterIdx).baseUrl;
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

    m_currentTask = m_downloadQueue.dequeue();
    qInfo() << "current task - " << m_currentTask.link;
    m_downloadPath.cd(m_recitersList.at(m_currentTask.reciterIdx).baseDirName);

    int sCount = m_dbPtr->getSurahVerseCount(m_currentTask.surah);

    while (m_downloadPath.exists(m_currentTask.link.fileName())) {
        emit downloadProgressed(m_currentTask.verse, sCount);

        if (m_currentTask.verse == sCount) {
            m_downloadPath.cdUp();
            emit downloadComplete();
        }

        if (m_downloadQueue.empty()) {
            m_isDownloading = false;
            return;
        }

        m_currentTask = m_downloadQueue.dequeue();
        qInfo() << "current task - " << m_currentTask.link;
    }

    m_isDownloading = true;
    QNetworkRequest req(m_currentTask.link);
    m_currentTask.networkReply = m_netMan->get(req);
}

void DownloadManager::finishupTask(QNetworkReply *replyData)
{
    if (m_currentTask.networkReply->error() != QNetworkReply::NoError) {
        handleConError(m_currentTask.networkReply->error());
        return;
    }

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
        qWarning() << "Couldn't open file:" << filename;
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

QList<Reciter> DownloadManager::recitersList() const
{
    return m_recitersList;
}

QNetworkAccessManager *DownloadManager::netMan() const
{
    return m_netMan;
}

DownloadManager::DownloadTask DownloadManager::currentTask() const
{
    return m_currentTask;
}
