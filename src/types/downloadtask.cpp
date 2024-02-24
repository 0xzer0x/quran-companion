#include "downloadtask.h"

DownloadTask::DownloadTask(const QUrl& url, const QFileInfo& dest)
  : m_url(url)
  , m_dest(dest)
{
}

QUrl
DownloadTask::url() const
{
  return m_url;
}

QFileInfo
DownloadTask::dest() const
{
  return m_dest;
}

QPointer<QNetworkReply>
DownloadTask::reply() const
{
  return m_reply;
}

void
DownloadTask::setReply(QPointer<QNetworkReply> newReply)
{
  m_reply = newReply;
}
