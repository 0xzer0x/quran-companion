#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject* parent,
                                 DBManager* dbptr,
                                 QList<Reciter> reciters)
  : QObject(parent)
  , m_recitersList{ reciters }
  , m_netMan{ new QNetworkAccessManager(this) }
  , m_dbMgr{ dbptr }
{
  m_topLevelPath.setPath(QDir::currentPath() + QDir::separator() +
                         "recitations");
  m_netMan->setTransferTimeout(3000);
  connect(m_netMan,
          &QNetworkAccessManager::finished,
          this,
          &DownloadManager::finishupTask,
          Qt::UniqueConnection);
}

void
DownloadManager::startQueue()
{
  if (!m_isDownloading) {
    processQueueHead();
    emit downloadStarted();
  }
}

void
DownloadManager::stopQueue()
{
  if (m_isDownloading) {
    cancelCurrentTask();
    m_isDownloading = false;
    m_downloadQueue.clear();
  }
}

void
DownloadManager::cancelCurrentTask()
{
  if (m_currentTask.networkReply == nullptr)
    return;

  m_currentTask.networkReply->abort();
  m_currentTask.networkReply->close();
  emit downloadCanceled();
}

void
DownloadManager::enqeueVerseTask(int reciterIdx, int surah, int verse)
{

  QUrl dl = downloadUrl(reciterIdx, surah, verse);
  DownloadTask t;
  t.surah = surah;
  t.verse = verse;
  t.reciterIdx = reciterIdx;
  t.link = dl;
  t.filename = QString::number(surah).rightJustified(3, '0') +
               QString::number(verse).rightJustified(3, '0') + ".mp3";

  m_downloadQueue.enqueue(t);
}

void
DownloadManager::processQueueHead()
{
  if (m_downloadQueue.empty()) {
    m_isDownloading = false;
    return;
  }

  m_currentTask = m_downloadQueue.dequeue();
  qInfo() << "current download task - " << m_currentTask.link;
  m_currSurahCount = m_dbMgr->getSurahVerseCount(m_currentTask.surah);

  m_downloadPath = m_topLevelPath;
  m_downloadPath.cd(m_recitersList.at(m_currentTask.reciterIdx).baseDirName);

  qInfo() << m_currentTask.filename;
  while (m_downloadPath.exists(m_currentTask.filename)) {
    emit downloadProgressed(m_currentTask.verse, m_currSurahCount);

    if (m_currentTask.verse == m_currSurahCount) {
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
  req.setTransferTimeout(3000);
  m_currentTask.networkReply = m_netMan->get(req);
  m_currentTask.networkReply->ignoreSslErrors();
  m_downloadStart = QTime::currentTime();

  connect(m_currentTask.networkReply,
          &QNetworkReply::downloadProgress,
          this,
          &DownloadManager::downloadProgress);
}

void
DownloadManager::downloadProgress(qint64 bytes, qint64 total)
{
  int secs = m_downloadStart.secsTo(QTime::currentTime());
  if (secs < 1)
    secs = 1;

  int speedPerSec = bytes / secs;
  QString unit = tr("bytes");
  if (speedPerSec >= 1024) {
    unit = tr("KB");
    speedPerSec /= 1024;
  }

  if (speedPerSec >= 1024) {
    unit = tr("MB");
    speedPerSec /= 1024;
  }

  emit downloadSpeedUpdated(speedPerSec, unit);
}

void
DownloadManager::finishupTask(QNetworkReply* replyData)
{
  if (m_currentTask.networkReply->error() != QNetworkReply::NoError) {
    handleConError(m_currentTask.networkReply->error());
    return;
  }

  saveFile(replyData);

  emit downloadProgressed(m_currentTask.verse, m_currSurahCount);
  if (m_currentTask.verse == m_currSurahCount) {
    emit downloadComplete();
  }

  disconnect(m_currentTask.networkReply,
             &QNetworkReply::downloadProgress,
             this,
             &DownloadManager::downloadProgress);
  m_currentTask.clear();

  if (!m_downloadQueue.isEmpty())
    processQueueHead();
}

bool
DownloadManager::saveFile(QNetworkReply* data)
{
  QFile localFile(m_downloadPath.filePath(m_currentTask.filename));

  if (!localFile.open(QIODevice::WriteOnly)) {
    qWarning() << "Couldn't open file:" << m_currentTask.filename;
    return false;
  }

  const QByteArray fdata = data->readAll();
  m_currentTask.networkReply->close();

  localFile.write(fdata);
  localFile.close();

  return true;
}

QUrl
DownloadManager::downloadUrl(const int reciterIdx,
                             const int surah,
                             const int verse) const
{
  Reciter r = m_recitersList.at(reciterIdx);
  QString url = r.baseUrl;
  if (r.useId)
    url.append(QString::number(m_dbMgr->getVerseId(surah, verse)) + ".mp3");
  else
    url.append(QString::number(surah).rightJustified(3, '0') +
               QString::number(verse).rightJustified(3, '0') + ".mp3");

  return QUrl::fromEncoded(url.toLocal8Bit());
}

bool
DownloadManager::isDownloading() const
{
  return m_isDownloading;
}

void
DownloadManager::handleConError(QNetworkReply::NetworkError err)
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

QList<Reciter>
DownloadManager::recitersList() const
{
  return m_recitersList;
}

QNetworkAccessManager*
DownloadManager::netMan() const
{
  return m_netMan;
}

DownloadManager::DownloadTask
DownloadManager::currentTask() const
{
  return m_currentTask;
}
