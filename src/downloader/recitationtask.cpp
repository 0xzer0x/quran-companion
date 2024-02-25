#include "recitationtask.h"

RecitationTask::RecitationTask()
  : m_reciter(-1)
  , m_surah(-1)
  , m_verse(-1)
{
}

RecitationTask::RecitationTask(int reciter, int surah, int verse)
  : m_reciter(reciter)
  , m_surah(surah)
  , m_verse(verse)
{
}

RecitationTask::RecitationTask(const RecitationTask& other)
{
  m_reciter = other.m_reciter;
  m_surah = other.m_surah;
  m_verse = other.m_verse;
}

RecitationTask::RecitationTask(const RecitationTask&& other)
{
  m_reciter = other.m_reciter;
  m_surah = other.m_surah;
  m_verse = other.m_verse;
}

RecitationTask&
RecitationTask::operator=(RecitationTask other)
{
  m_reciter = other.m_reciter;
  m_surah = other.m_surah;
  m_verse = other.m_verse;
  return *this;
}

QUrl
RecitationTask::url() const
{
  const Reciter& r = *m_reciters.at(m_reciter);
  QString url = r.baseUrl();
  if (r.useId())
    url.append(QString::number(m_quranDb->verseId(m_surah, m_verse)) + ".mp3");
  else
    url.append(QString::number(m_surah).rightJustified(3, '0') +
               QString::number(m_verse).rightJustified(3, '0') + ".mp3");

  return QUrl::fromEncoded(url.toLatin1());
}

QFileInfo
RecitationTask::destination() const
{
  static const QString path = "recitations/%0/%1.mp3";
  return QFileInfo(m_downloadsDir->absoluteFilePath(
    path.arg(m_reciters.at(m_reciter)->baseDirName(),
             QString::number(m_surah).rightJustified(3, '0') +
               QString::number(m_verse).rightJustified(3, '0'))));
}

RecitationTask::~RecitationTask() {}
