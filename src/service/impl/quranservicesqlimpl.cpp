#include "quranservicesqlimpl.h"

QuranServiceSqlImpl::QuranServiceSqlImpl()
  : m_quranRepository(QuranRepository::getInstance())
{
}

QPair<int, int>
QuranServiceSqlImpl::pageMetadata(const int page) const
{
  return m_quranRepository.pageMetadata(page);
}

int
QuranServiceSqlImpl::getVersePage(const int& surahIdx, const int& verse) const
{
  return m_quranRepository.getVersePage(surahIdx, verse);
}

int
QuranServiceSqlImpl::getJuzStartPage(const int juz) const
{
  return m_quranRepository.getJuzStartPage(juz);
}

int
QuranServiceSqlImpl::getJuzOfPage(const int page) const
{
  return m_quranRepository.getJuzOfPage(page);
}

QList<QList<int>>
QuranServiceSqlImpl::verseInfoList(const int page) const
{
  return m_quranRepository.verseInfoList(page);
}

QString
QuranServiceSqlImpl::verseText(const int sIdx, const int vIdx) const
{
  return m_quranRepository.verseText(sIdx, vIdx);
}

int
QuranServiceSqlImpl::surahStartPage(int surahIdx) const
{
  return m_quranRepository.surahStartPage(surahIdx);
}

QString
QuranServiceSqlImpl::surahName(const int sIdx, bool ar) const
{
  return m_quranRepository.surahName(sIdx, ar);
}

QList<int>
QuranServiceSqlImpl::verseById(const int id) const
{
  return m_quranRepository.verseById(id);
}

int
QuranServiceSqlImpl::versePage(const int& surahIdx, const int& verse) const
{
  return m_quranRepository.versePage(surahIdx, verse);
}

QList<int>
QuranServiceSqlImpl::searchSurahNames(QString text) const
{
  return m_quranRepository.searchSurahNames(text);
}

QList<QList<int>>
QuranServiceSqlImpl::searchSurahs(QString searchText,
                                  const QList<int> surahs,
                                  const bool whole) const
{
  return m_quranRepository.searchSurahs(searchText, surahs, whole);
}

QList<QList<int>>
QuranServiceSqlImpl::searchVerses(QString searchText,
                                  const int range[],
                                  const bool whole) const
{
  return m_quranRepository.searchVerses(searchText, range, whole);
}

QList<int>
QuranServiceSqlImpl::randomVerse() const
{
  return m_quranRepository.randomVerse();
}

QStringList
QuranServiceSqlImpl::surahNames() const
{
  return m_quranRepository.surahNames();
}

Verse
QuranServiceSqlImpl::next(const Verse& verse, bool withBasmallah) const
{
  return m_quranRepository.next(verse, withBasmallah);
}

Verse
QuranServiceSqlImpl::previous(const Verse& verse, bool withBasmallah) const
{
  return m_quranRepository.previous(verse, withBasmallah);
}
