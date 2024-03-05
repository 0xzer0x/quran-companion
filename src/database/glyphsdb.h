#ifndef GLYPHSDB_H
#define GLYPHSDB_H

#include <QDir>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class GlyphsDb
 * @brief The GlyphsDb class represents a connection to the glyphs db file
 */
class GlyphsDb
  : public DbConnection
  , QSqlDatabase
{
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static GlyphsDb& getInstance();
  /**
   * @brief sets and opens the sqlite database file
   */
  void open();
  /**
   * @brief getter for the type of the connection
   * @return - DbConnection::Glyphs
   */
  Type type();
  /**
   * @brief get Quran page QCF glyphs separated as lines
   * @param page - Quran page number
   * @return QList of page lines
   */
  QStringList getPageLines(const int page) const;
  /**
   * @brief gets the surah name glyph for the QCF_BSML font, used to render
   * surah frame in Quran page
   * @param sura - sura number (1-114)
   * @return QString of glyphs
   */
  QString getSurahNameGlyph(const int sura) const;
  /**
   * @brief gets the juz name in arabic, used in page header
   * @param juz - juz number
   * @return QString of the juz name
   */
  QString getJuzGlyph(const int juz) const;
  /**
   * @brief gets the verse QCF glyphs for the corresponding QCF page font
   * @param sIdx - sura number (1-114)
   * @param vIdx - verse number
   * @return QString of verse glyphs
   */
  QString getVerseGlyphs(const int sIdx, const int vIdx) const;

private:
  GlyphsDb();
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;
  /**
   * @brief reference to the app assets directory
   */
  const QDir& m_assetsDir;
};

#endif // GLYPHSDB_H
