#include "verse.h"

Verse*
Verse::current()
{
  static Verse m_current = Verse();
  return &m_current;
}

QList<Verse>
Verse::fromList(QList<QList<int>> lst)
{
  QList<Verse> final(lst.size());
  for (int i = 0; i < lst.size(); i++)
    final[i].update(lst[i]);

  return final;
}

Verse::Verse() {}

Verse::Verse(int page, int surah, int number)
  : m_page(page)
  , m_number(number)
{
  setSurah(surah);
}

Verse::Verse(const QList<int> vInfo)
{
  m_page = vInfo[0];
  setSurah(vInfo[1]);
  m_number = vInfo[2];
}

Verse&
Verse::operator=(const QList<int>& vInfo)
{
  m_page = vInfo[0];
  setSurah(vInfo[1]);
  m_number = vInfo[2];
  return *this;
}

Verse&
Verse::operator=(const Verse& cp)
{
  m_page = cp.m_page;
  m_surah = cp.m_surah;
  m_surahCount = cp.m_surahCount;
  m_number = cp.m_number;
  return *this;
}

bool
Verse::operator==(const Verse& v2) const
{
  return (m_number == v2.m_number && m_surah == v2.m_surah);
}

bool
Verse::operator!=(const Verse& v2) const
{
  return (m_number != v2.m_number || m_surah != v2.m_surah);
}

bool
Verse::operator<(const Verse& v2) const
{
  if (m_surah == v2.surah())
    return m_number < v2.m_number;

  return m_surah < v2.m_surah;
}

bool
Verse::operator>(const Verse& v2) const
{
  if (m_surah == v2.m_surah)
    return m_number > v2.m_number;

  return m_surah > v2.m_surah;
}

void
Verse::update(const Verse& v)
{
  m_page = v.m_page;
  setSurah(v.m_surah);
  m_number = v.m_number;
}

void
Verse::update(const QList<int>& vInfo)
{
  m_page = vInfo[0];
  setSurah(vInfo[1]);
  m_number = vInfo[2];
}

Verse
Verse::next() const
{
  QList<int> vInfo =
    m_dbMgr->getVerseById(m_dbMgr->getVerseId(m_surah, m_number) + 1);
  return Verse(vInfo);
}

Verse
Verse::prev() const
{
  QList<int> vInfo =
    m_dbMgr->getVerseById(m_dbMgr->getVerseId(m_surah, m_number) - 1);
  return Verse(vInfo);
}

void
Verse::updateSurahCount()
{
  m_surahCount = m_dbMgr->getSurahVerseCount(m_surah);
}

void
Verse::setPage(int newPage)
{
  m_page = newPage;
}

void
Verse::setSurah(int newSurah)
{
  if (m_surah == newSurah)
    return;
  m_surah = newSurah;
  updateSurahCount();
}

void
Verse::setNumber(int newNumber)
{
  m_number = newNumber;
}

QList<int>
Verse::toList() const
{
  return { m_page, m_surah, m_number };
}

int
Verse::surahCount() const
{
  return m_surahCount;
}

int
Verse::page() const
{
  return m_page;
}

int
Verse::surah() const
{
  return m_surah;
}

int
Verse::number() const
{
  return m_number;
}
