#include "glyphsdb.h"
#include <QSqlQuery>

GlyphsDb&
GlyphsDb::getInstance()
{
  static GlyphsDb gdb;
  return gdb;
}

GlyphsDb::GlyphsDb()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "GlyphsCon"))
  , m_config(Configuration::getInstance())
  , m_assetsDir(DirManager::getInstance().assetsDir())
{
  GlyphsDb::open();
}

void
GlyphsDb::open()
{
  setDatabaseName(m_assetsDir.absoluteFilePath("glyphs.db"));
  if (!QSqlDatabase::open())
    qFatal("Error opening glyphs db");
}

DbConnection::Type
GlyphsDb::type()
{
  return DbConnection::Glyphs;
}

QStringList
GlyphsDb::getPageLines(const int page) const
{
  QSqlQuery dbQuery(*this);

  QString query = "SELECT %0 FROM pages WHERE page_no=%1";
  query = query.arg("qcf_v" + QString::number(m_config.qcfVersion()),
                    QString::number(page));

  dbQuery.prepare(query);
  if (!dbQuery.exec())
    qFatal("Couldn't execute getPageLines query!");

  dbQuery.next();
  QStringList lines = dbQuery.value(0).toString().trimmed().split('\n');

  return lines;
}

QString
GlyphsDb::getSurahNameGlyph(const int sura) const
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare("SELECT qcf_v1 FROM surah_glyphs WHERE surah=:i");
  dbQuery.bindValue(0, sura);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getSurahNameGlyph SQL statment exec";
  }

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QString
GlyphsDb::getJuzGlyph(const int juz) const
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare("SELECT text FROM juz_glyphs WHERE juz=:j");
  dbQuery.bindValue(0, juz);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getJuzGlyph SQL statment exec";
  }

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QString
GlyphsDb::getVerseGlyphs(const int sIdx, const int vIdx) const
{
  QSqlQuery dbQuery(*this);

  QString query = "SELECT %0 FROM ayah_glyphs WHERE surah=%1 AND ayah=%2";
  query = query.arg("qcf_v" + QString::number(m_config.qcfVersion()),
                    QString::number(sIdx),
                    QString::number(vIdx));

  dbQuery.prepare(query);
  if (!dbQuery.exec())
    qFatal("Couldn't execute getVerseGlyphs query!");

  dbQuery.next();

  return dbQuery.value(0).toString();
}
