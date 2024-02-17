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
DownloadManager::addToQueue(DownloadType type, QPair<int, int> info)
{
  m_downloadQueue.enqueue(QPair<DownloadType, QPair<int, int>>(type, info));
}

void
DownloadManager::addToQueue(int reciter, int surah)
{
  m_downloadQueue.enqueue(QPair<DownloadType, QPair<int, int>>(
    Recitation, QPair<int, int>(reciter, surah)));
}

void
DownloadManager::startQueue()
{
  if (!m_isDownloading) {
    processTaskQueue();
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
    m_taskQueue.clear();
  }
}

void
DownloadManager::cancelCurrentTask()
{
  if (m_activeTask.reply == nullptr)
    return;

  m_activeTask.reply->abort();
  m_activeTask.reply->close();
  emit downloadCanceled();
}

void
DownloadManager::enqeueQCF()
{
  static const QString base =
    "https://github.com/0xzer0x/quran-companion/raw/main/extras/";
  QString path;
  DownloadTask t;
  for (int i = 1; i <= 604; i++) {
    path = QString("QCFV2/QCF2%0.ttf")
             .arg(QString::number(i).rightJustified(3, '0'));
    t.metainfo = { -1, -1, i };
    t.metainfo.squeeze();
    t.downloadPath.setFile(m_downloadsDir->absoluteFilePath(path));
    t.link = QUrl::fromEncoded((base + path).toLatin1());
    m_taskQueue.enqueue(t);
  }
}

void
DownloadManager::enqeueTask(QPair<int, int> info)
{
  static const QString base =
    "https://github.com/0xzer0x/quran-companion/raw/main/extras/";
  QString path;
  if (info.first)
    path = "translations/" + m_translations.at(info.second)->filename();
  else
    path = "tafasir/" + m_tafasir.at(info.second)->filename();
  DownloadTask t;
  t.metainfo = { info.first, info.second, 0 };
  t.metainfo.squeeze();
  t.link = QUrl::fromEncoded((base + path).toLatin1());
  t.downloadPath.setFile(m_downloadsDir->absoluteFilePath(path));
  m_taskQueue.enqueue(t);
}

void
DownloadManager::enqeueTask(int reciterIdx, int surah, int verse)
{
  static const QString path = "recitations/%0/%1.mp3";
  DownloadTask t;
  t.metainfo = { reciterIdx, surah, verse };
  t.metainfo.squeeze();
  t.link = downloadUrl(reciterIdx, surah, verse);
  t.downloadPath.setFile(m_downloadsDir->absoluteFilePath(
    path.arg(m_recitersList.at(reciterIdx)->baseDirName(),
             QString::number(surah).rightJustified(3, '0') +
               QString::number(verse).rightJustified(3, '0'))));

  m_taskQueue.enqueue(t);
}

void
DownloadManager::processDownloadQueue()
{
  if (m_downloadQueue.empty()) {
    m_isDownloading = false;
    return;
  }

  m_isDownloading = true;
  QPair<DownloadType, QPair<int, int>> current = m_downloadQueue.dequeue();
  QPair<int, int>& info = current.second;
  m_activeType = current.first;
  if (current.first == QCF) {
    m_activeTotal = 604;
    enqeueQCF();
  } else if (current.first == Recitation) {
    m_activeTotal = m_dbMgr->getSurahVerseCount(info.second);
    for (int v = 1; v <= m_activeTotal; v++)
      enqeueTask(info.first, info.second, v);
  } else if (current.first == File) {
    m_activeTotal = 1;
    enqeueTask(info);
  }
}

void
DownloadManager::processTaskQueue()
{
  if (m_taskQueue.empty()) {
    processDownloadQueue();
    if (!m_isDownloading)
      return;
  }

  m_activeTask = m_taskQueue.dequeue();

  while (m_activeTask.downloadPath.exists()) {
    if (m_activeTask.metainfo[2] == m_activeTotal || m_activeType == File) {
      emit downloadProgressed(m_activeTotal, m_activeTotal);
      emit filesFound(m_activeType, m_activeTask.metainfo);
    }

    if (m_taskQueue.empty()) {
      processDownloadQueue();
      if (!m_isDownloading)
        return;
    }

    m_activeTask = m_taskQueue.dequeue();
  }

  QNetworkRequest req(m_activeTask.link);
  m_activeTask.reply = m_netMan->get(req);
  m_activeTask.reply->ignoreSslErrors();
  m_downloadStart = QTime::currentTime();

  connect(m_activeTask.reply,
          &QNetworkReply::downloadProgress,
          this,
          &DownloadManager::downloadProgress);
}

void
DownloadManager::downloadProgress(qint64 bytes, qint64 total)
{
  if (m_activeType == File) {
    if (!m_activeTask.metainfo[2])
      m_activeTotal = total / 1024;
    m_activeTask.metainfo[2] = bytes / 1024;
    emit downloadProgressed(m_activeTask.metainfo[2], m_activeTotal);
  }

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

  if (m_activeTask.reply->error() != QNetworkReply::NoError)
    return handleConError(m_activeTask.reply->error());

  saveFile(replyData);

  emit downloadProgressed(m_activeTask.metainfo[2], m_activeTotal);
  if (m_activeTask.metainfo[2] == m_activeTotal) {
    emit downloadCompleted(m_activeType, m_activeTask.metainfo);
  }

  disconnect(m_activeTask.reply,
             &QNetworkReply::downloadProgress,
             this,
             &DownloadManager::downloadProgress);

  processTaskQueue();
}

bool
DownloadManager::saveFile(QNetworkReply* data)
{
  QFile localFile(m_activeTask.downloadPath.absoluteFilePath());

  if (!localFile.open(QIODevice::WriteOnly)) {
    qWarning() << "Couldn't open file:" << m_activeTask.downloadPath;
    return false;
  }

  const QByteArray fdata = data->readAll();
  m_activeTask.reply->close();

  localFile.write(fdata);
  localFile.close();

  return true;
}

QUrl
DownloadManager::downloadUrl(const int reciterIdx,
                             const int surah,
                             const int verse) const
{
  const Reciter& r = *m_recitersList.at(reciterIdx);
    QString url = r.baseUrl();
  if (r.useId())
    url.append(QString::number(m_dbMgr->getVerseId(surah, verse)) + ".mp3");
  else
    url.append(QString::number(surah).rightJustified(3, '0') +
               QString::number(verse).rightJustified(3, '0') + ".mp3");

  return QUrl::fromEncoded(url.toLatin1());
}

void
DownloadManager::handleConError(QNetworkReply::NetworkError err)
{
  switch (err) {
    case QNetworkReply::OperationCanceledError:
      qInfo() << m_activeTask.reply->errorString();
      break;

    default:
      qInfo() << m_activeTask.reply->errorString();
      emit downloadErrored(m_activeType, m_activeTask.metainfo);
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

DownloadManager::DownloadTask
DownloadManager::currentTask() const
{
  return m_activeTask;
}

bool
DownloadManager::isDownloading() const
{
  return m_isDownloading;
}
