#include "tafsirtask.h"

TafsirTask::TafsirTask(int idx)
  : m_idx(idx)
{
}

TafsirTask::TafsirTask(const TafsirTask& other)
{
  m_idx = other.m_idx;
}

TafsirTask::TafsirTask(const TafsirTask&& other)
{
  m_idx = other.m_idx;
}

TafsirTask&
TafsirTask::operator=(const TafsirTask other)
{
  m_idx = other.m_idx;
  return *this;
}

QUrl
TafsirTask::url() const
{
  return QUrl::fromEncoded(
    ("https://github.com/0xzer0x/quran-companion/raw/main/extras/tafasir/" +
     m_tafasir.at(m_idx)->filename())
      .toLatin1());
}

QFileInfo
TafsirTask::destination() const
{
  return QFileInfo(m_downloadsDir->absoluteFilePath(
    "tafasir/" + m_tafasir.at(m_idx)->filename()));
}
