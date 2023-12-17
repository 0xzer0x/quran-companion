/**
 * @file downloadmanager.cpp
 * @brief Implementation file for DownloadManager
 */

#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject* parent)
  : QObject(parent)
  , m_netMan{ new QNetworkAccessManager(this) }
{
  connect(m_netMan,
          &QNetworkAccessManager::finished,
          this,
          &DownloadManager::finishupTask);

  m_versionReq.setUrl(QUrl::fromEncoded(
    "https://raw.githubusercontent.com/0xzer0x/quran-companion/main/VERSION"));
  m_versionReq.setTransferTimeout(1500);
  m_versionReq.setAttribute(QNetworkRequest::User, 1);
}

void
DownloadManager::getLatestVersion()
{
  m_versionReply = m_netMan->get(m_versionReq);
  m_versionReply->ignoreSslErrors();
}

void
DownloadManager::addSurahToQueue(int reciter, int surah)
{
  m_surahQueue.enqueue(QPair<int, int>(reciter, surah));
}

void
DownloadManager::startQueue()
{
  if (!m_isDownloading) {
    processDownloadQueue();
    emit downloadStarted();
  }
}

void
DownloadManager::stopQueue()
{
  if (m_isDownloading) {
    cancelCurrentTask();
    m_isDownloading = false;
    m_surahQueue.clear();
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
  VerseTask t;
  t.surah = surah;
  t.verse = verse;
  t.reciterIdx = reciterIdx;
  t.link = downloadUrl(reciterIdx, surah, verse);
  t.downloadPath.setFile(m_toplevelDownloadPath.absoluteFilePath(
    m_recitersList.at(reciterIdx).baseDirName + QDir::separator() +
    QString::number(surah).rightJustified(3, '0') +
    QString::number(verse).rightJustified(3, '0') + ".mp3"));

  m_downloadQueue.enqueue(t);
}

void
DownloadManager::processSurahQueue()
{
  if (m_surahQueue.empty()) {
    m_isDownloading = false;
    return;
  }

  m_isDownloading = true;
  QPair<int, int> current = m_surahQueue.dequeue();
  m_currSurahCount = m_dbMgr->getSurahVerseCount(current.second);
  for (int v = 1; v <= m_currSurahCount; v++)
    enqeueVerseTask(current.first, current.second, v);
}

void
DownloadManager::processDownloadQueue()
{
  if (m_downloadQueue.empty()) {
    processSurahQueue();
    if (!m_isDownloading)
      return;
  }

  m_currentTask = m_downloadQueue.dequeue();

  while (m_currentTask.downloadPath.exists()) {
    if (m_currentTask.verse == m_currSurahCount) {
      emit downloadProgressed(m_currSurahCount, m_currSurahCount);
      emit surahFound(m_currentTask.reciterIdx, m_currentTask.surah);
    }

    if (m_downloadQueue.empty()) {
      processSurahQueue();
      if (!m_isDownloading)
        return;
    }

    m_currentTask = m_downloadQueue.dequeue();
  }

  QNetworkRequest req(m_currentTask.link);
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
  if (replyData->request().attribute(QNetworkRequest::User).toInt() == 1)
    return handleVersionReply();

  if (m_currentTask.networkReply->error() != QNetworkReply::NoError)
    return handleConError(m_currentTask.networkReply->error());

  saveFile(replyData);

  emit downloadProgressed(m_currentTask.verse, m_currSurahCount);
  if (m_currentTask.verse == m_currSurahCount)
    emit downloadComplete(m_currentTask.reciterIdx, m_currentTask.surah);

  disconnect(m_currentTask.networkReply,
             &QNetworkReply::downloadProgress,
             this,
             &DownloadManager::downloadProgress);
  m_currentTask.clear();

  processDownloadQueue();
}

bool
DownloadManager::saveFile(QNetworkReply* data)
{
  QFile localFile(m_currentTask.downloadPath.absoluteFilePath());

  if (!localFile.open(QIODevice::WriteOnly)) {
    qWarning() << "Couldn't open file:" << m_currentTask.downloadPath;
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
  const Reciter& r = m_recitersList.at(reciterIdx);
  QString url = r.baseUrl;
  if (r.useId)
    url.append(QString::number(m_dbMgr->getVerseId(surah, verse)) + ".mp3");
  else
    url.append(QString::number(surah).rightJustified(3, '0') +
               QString::number(verse).rightJustified(3, '0') + ".mp3");

  return QUrl::fromEncoded(url.toLocal8Bit());
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
      emit downloadError(m_currentTask.reciterIdx, m_currentTask.surah);
  }
}

void
DownloadManager::handleVersionReply()
{
  int status =
    m_versionReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  if (status == 200)
    emit latestVersionFound(m_versionReply->readAll().trimmed());
}

QNetworkAccessManager*
DownloadManager::netMan() const
{
  return m_netMan;
}

DownloadManager::VerseTask
DownloadManager::currentTask() const
{
  return m_currentTask;
}

bool
DownloadManager::isDownloading() const
{
  return m_isDownloading;
}
