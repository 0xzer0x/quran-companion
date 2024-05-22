#ifndef GLYPHSERVICESQLIMPL_H
#define GLYPHSERVICESQLIMPL_H

#include <repository/glyphsrepository.h>
#include <service/glyphservice.h>

class GlyphServiceSqlImpl : public GlyphService
{
private:
  GlyphsRepository& m_glyphRepository;

public:
  GlyphServiceSqlImpl();

  QStringList getPageLines(const int page) const override;

  QString getSurahNameGlyph(const int sura) const override;

  QString getJuzGlyph(const int juz) const override;

  QString getVerseGlyphs(const int sIdx, const int vIdx) const override;
};

#endif // GLYPHSERVICESQLIMPL_H
