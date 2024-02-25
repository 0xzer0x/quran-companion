#ifndef GLYPHSDB_H
#define GLYPHSDB_H

#include <QDir>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <utils/dirmanager.h>
#include <utils/settings.h>

class GlyphsDb
  : public DbConnection
  , QSqlDatabase
{
public:
    static QSharedPointer<GlyphsDb> current();
  GlyphsDb();
  void open();
  Type type();
  /**
   * @brief get Quran page QCF glyphs separated as lines
   * @param page - Quran page number
   * @return QList of page lines
   */
  QStringList getPageLines(const int page);
  /**
   * @brief gets the surah name glyph for the QCF_BSML font, used to render
   * surah frame in Quran page
   * @param sura - sura number (1-114)
   * @return QString of glyphs
   */
  QString getSurahNameGlyph(const int sura);
  /**
   * @brief gets the juz name in arabic, used in page header
   * @param juz - juz number
   * @return QString of the juz name
   */
  QString getJuzGlyph(const int juz);
  /**
   * @brief gets the verse QCF glyphs for the corresponding QCF page font
   * @param sIdx - sura number (1-114)
   * @param vIdx - verse number
   * @return QString of verse glyphs
   */
  QString getVerseGlyphs(const int sIdx, const int vIdx);

private:
  const int m_qcfVer = Settings::qcfVersion;
  const QSharedPointer<QDir> m_assetsDir = DirManager::assetsDir;
};

#endif // GLYPHSDB_H
