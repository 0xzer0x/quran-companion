#ifndef GLYPH_SERVICE_H
#define GLYPH_SERVICE_H

#include <QString>
#include <QStringList>

class GlyphService
{
public:
  /**
   * @brief get Quran page QCF glyphs separated as lines
   * @param page - Quran page number
   * @return QList of page lines
   */
  virtual QStringList getPageLines(const int page) const = 0;
  /**
   * @brief gets the surah name glyph for the QCF_BSML font, used to render
   * surah frame in Quran page
   * @param sura - sura number (1-114)
   * @return QString of glyphs
   */
  virtual QString getSurahNameGlyph(const int sura) const = 0;
  /**
   * @brief gets the juz name in arabic, used in page header
   * @param juz - juz number
   * @return QString of the juz name
   */
  virtual QString getJuzGlyph(const int juz) const = 0;
  /**
   * @brief gets the verse QCF glyphs for the corresponding QCF page font
   * @param sIdx - sura number (1-114)
   * @param vIdx - verse number
   * @return QString of verse glyphs
   */
  virtual QString getVerseGlyphs(const int sIdx, const int vIdx) const = 0;
};

#endif
