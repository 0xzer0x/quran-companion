#ifndef GLYPHSREPOSITORY_H
#define GLYPHSREPOSITORY_H

#include <QDir>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <repository/dbconnection.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class GlyphsRepository
 * @brief The GlyphsRepository class represents a connection to the glyphs db file
 */
class GlyphsRepository
  : public DbConnection
  , QSqlDatabase
{
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
    static GlyphsRepository& getInstance();

  void open() override;

  Type type() override;

  QStringList getPageLines(const int page) const;

  QString getSurahNameGlyph(const int sura) const;

  QString getJuzGlyph(const int juz) const;

  QString getVerseGlyphs(const int sIdx, const int vIdx) const;

private:
  GlyphsRepository();
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;
  /**
   * @brief reference to the app assets directory
   */
  const QDir& m_assetsDir;
};

#endif // GLYPHSREPOSITORY_H
