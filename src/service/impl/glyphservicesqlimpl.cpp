#include "glyphservicesqlimpl.h"

GlyphServiceSqlImpl::GlyphServiceSqlImpl()
  : m_glyphRepository(GlyphsRepository::getInstance())
{
}

QStringList
GlyphServiceSqlImpl::getPageLines(const int page) const
{
  return m_glyphRepository.getPageLines(page);
}

QString
GlyphServiceSqlImpl::getSurahNameGlyph(const int sura) const
{
  return m_glyphRepository.getSurahNameGlyph(sura);
}

QString
GlyphServiceSqlImpl::getJuzGlyph(const int juz) const
{
  return m_glyphRepository.getJuzGlyph(juz);
}

QString
GlyphServiceSqlImpl::getVerseGlyphs(const int sIdx, const int vIdx) const
{
  return m_glyphRepository.getVerseGlyphs(sIdx, vIdx);
}
