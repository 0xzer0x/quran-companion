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
 * @brief Manages a connection to the glyphs database for accessing various
 * Quranic glyphs.
 *
 * The `GlyphsRepository` class provides methods to access glyphs data from the
 * glyphs database file
 * (`glyphs.db`). It includes methods for retrieving page lines, surah name
 * glyphs, juz glyphs, and verse glyphs.
 */
class GlyphsRepository
  : public DbConnection
  , QSqlDatabase
{
public:
  /**
   * @brief Provides access to the singleton instance of GlyphsRepository.
   * @return Reference to the static class instance.
   */
  static GlyphsRepository& getInstance();
  /**
   * @brief Opens the connection to the glyphs database and initializes the
   * database schema.
   */
  void open() override;
  /**
   * @brief Returns the type of the database connection.
   * @return Type of the database connection as DbConnection::Glyphs.
   */
  Type type() override;
  /**
   * @brief Retrieves the lines of a specific page from the glyphs database.
   * @param page The page number to retrieve lines for.
   * @return QStringList containing the lines of the specified page.
   */
  QStringList getPageLines(const int page) const;
  /**
   * @brief Retrieves the glyph for a specific surah name.
   * @param sura The surah number to retrieve the glyph for.
   * @return QString containing the glyph for the specified surah name.
   */
  QString getSurahNameGlyph(const int sura) const;
  /**
   * @brief Retrieves the glyph for a specific juz.
   * @param juz The juz number to retrieve the glyph for.
   * @return QString containing the glyph for the specified juz.
   */
  QString getJuzGlyph(const int juz) const;
  /**
   * @brief Retrieves the glyphs for a specific verse.
   * @param sIdx The surah index of the verse.
   * @param vIdx The verse index of the verse.
   * @return QString containing the glyphs for the specified verse.
   */
  QString getVerseGlyphs(const int sIdx, const int vIdx) const;

private:
  /**
   * @brief Constructor for GlyphsRepository.
   *
   * Initializes the GlyphsRepository instance and opens the database
   * connection.
   */
  GlyphsRepository();
  /**
   * @brief Reference to the singleton Configuration instance.
   */
  Configuration& m_config;
  /**
   * @brief Reference to the application assets directory.
   */
  const QDir& m_assetsDir;
};

#endif // GLYPHSREPOSITORY_H
