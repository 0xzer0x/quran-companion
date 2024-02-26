#include "verse.h"

const QList<int> Verse::verseCount = {
  7,   286, 200, 176, 120, 165, 206, 75,  129, 109, 123, 111, 43, 52, 99,
  128, 111, 110, 98,  135, 112, 78,  118, 64,  77,  227, 93,  88, 69, 60,
  34,  30,  73,  54,  45,  83,  182, 88,  75,  85,  54,  53,  89, 59, 37,
  35,  38,  29,  18,  45,  60,  49,  62,  55,  78,  96,  29,  22, 24, 13,
  14,  11,  11,  18,  12,  12,  30,  52,  52,  44,  28,  28,  20, 56, 40,
  31,  50,  40,  46,  42,  29,  19,  36,  25,  22,  17,  19,  26, 30, 20,
  15,  21,  11,  8,   8,   19,  5,   8,   8,   11,  11,  8,   3,  9,  5,
  4,   7,   3,   6,   3,   5,   4,   5,   6
};

const int
Verse::surahVerseCount(int surah)
{
  if (surah > 114 || surah < 1)
    return 0;
  return verseCount.at(surah - 1);
}

QSharedPointer<Verse>
Verse::current()
{
  static QSharedPointer<Verse> current = QSharedPointer<Verse>::create();
  return current;
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
Verse::next(bool basmalah)
{
  if (!m_number) {
    m_number = 1;
    return *this;
  }

  QList<int> vInfo =
    m_quranDb->verseById(m_quranDb->verseId(m_surah, m_number) + 1);

  if (vInfo[2] == 1 && vInfo[1] != 9 && vInfo[1] != 1 && basmalah)
    vInfo[2] = 0;

  return Verse(vInfo);
}

Verse
Verse::prev(bool basmalah)
{
  if (m_number == 1 && m_surah != 9 && m_surah != 1 && basmalah) {
    m_number = 0;
    return *this;
  }

  if (!m_number)
    m_number = 1;

  QList<int> vInfo =
    m_quranDb->verseById(m_quranDb->verseId(m_surah, m_number) - 1);
  return Verse(vInfo);
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
  m_surahCount = verseCount.at(m_surah - 1);
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
