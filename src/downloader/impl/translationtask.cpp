#include "translationtask.h"

TranslationTask::TranslationTask(int idx)
  : m_idx(idx)
{
}

TranslationTask::TranslationTask(const TranslationTask& other)
{
  m_idx = other.m_idx;
}

TranslationTask::TranslationTask(const TranslationTask&& other)
{
  m_idx = other.m_idx;
}

TranslationTask&
TranslationTask::operator=(const TranslationTask other)
{
  m_idx = other.m_idx;
  return *this;
}

QUrl
TranslationTask::url() const
{
  return QUrl::fromEncoded(
    ("https://github.com/0xzer0x/quran-companion-extras/raw/"
     "main/translations/" +
     m_translations.at(m_idx).filename())
      .toLatin1());
}

QFileInfo
TranslationTask::destination() const
{
  return QFileInfo(m_downloadsDir.absoluteFilePath(
    "translations/" + m_translations.at(m_idx).filename()));
}
