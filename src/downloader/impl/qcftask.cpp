#include "qcftask.h"

QcfTask::QcfTask(int page)
  : m_page(page)
{
}

QcfTask::QcfTask(const QcfTask& other)
{
  m_page = other.m_page;
}

QcfTask::QcfTask(const QcfTask&& other)
{
  m_page = other.m_page;
}

QcfTask&
QcfTask::operator=(const QcfTask other)
{
  m_page = other.m_page;
  return *this;
}

QUrl
QcfTask::url() const
{
  return QUrl::fromEncoded(
    ("https://github.com/0xzer0x/quran-companion-extras/raw/main/QCFV2/QCF2" +
     QString::number(m_page).rightJustified(3, '0') + ".ttf")
      .toLatin1());
}

QFileInfo
QcfTask::destination() const
{
  return QFileInfo(m_downloadsDir.absoluteFilePath(
    "QCFV2/QCF2" + QString::number(m_page).rightJustified(3, '0') + ".ttf"));
}
