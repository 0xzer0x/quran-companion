#include "taskdownloader.h"
#include <QApplication>

TaskDownloader::TaskDownloader(QObject* parent)
  : QObject(parent)
  , m_reply(nullptr)
  , m_task(nullptr)
  , m_bytes(0)
  , m_total(0)
{
}

void
TaskDownloader::process(DownloadTask* task, QNetworkAccessManager* manager)
{
  if (m_reply) {
    disconnect(m_reply,
               &QNetworkReply::downloadProgress,
               this,
               &TaskDownloader::taskProgress);
    disconnect(
      m_reply, &QNetworkReply::finished, this, &TaskDownloader::finish);
  }

  QNetworkRequest req(m_task->url());
  m_reply = manager->get(req);
  m_reply->ignoreSslErrors();
  m_startTime = QTime::currentTime();

  connect(m_reply,
          &QNetworkReply::downloadProgress,
          this,
          &TaskDownloader::taskProgress);
  connect(m_reply, &QNetworkReply::finished, this, &TaskDownloader::finish);
}

void
TaskDownloader::cancel()
{
  m_reply->abort();
  m_reply->close();
}

void
TaskDownloader::taskProgress(qint64 bytes, qint64 total)
{
  m_bytes = bytes;
  m_total = total;
  int secs = m_startTime.secsTo(QTime::currentTime());
  if (secs < 1)
    secs = 1;

  int speedPerSec = bytes / secs;
  QString unit = qApp->translate("DownloadManager", "bytes");
  if (speedPerSec >= 1024) {
    unit = qApp->translate("DownloadManager", "KB");
    speedPerSec /= 1024;
  }
  if (speedPerSec >= 1024) {
    unit = qApp->translate("DownloadManager", "MB");
    speedPerSec /= 1024;
  }

  emit downloadSpeedUpdated(speedPerSec, unit);
}

void
TaskDownloader::finish()
{
  if (m_reply->error() != QNetworkReply::NoError)
    return handleError(m_reply->error());
  saveFile();
  emit completed();
}

void
TaskDownloader::handleError(QNetworkReply::NetworkError err)
{
  switch (err) {
    case QNetworkReply::OperationCanceledError:
      qInfo() << m_reply->errorString();
      break;
    default:
      qInfo() << m_reply->errorString();
      emit taskError();
  }
}

bool
TaskDownloader::saveFile()
{
  QFile localFile(m_task->destination().absoluteFilePath());
  if (!localFile.open(QIODevice::WriteOnly)) {
    qWarning() << "Couldn't open file:" << m_task->destination();
    return false;
  }

  const QByteArray fdata = m_reply->readAll();
  m_reply->close();

  localFile.write(fdata);
  localFile.close();
  return true;
}

int
TaskDownloader::bytes() const
{
  return m_bytes;
}

int
TaskDownloader::total() const
{
  return m_total;
}
