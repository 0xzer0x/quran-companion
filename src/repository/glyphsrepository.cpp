#include "glyphsrepository.h"
#include <QSqlError>
#include <QSqlQuery>

GlyphsRepository&
GlyphsRepository::getInstance()
{
  static GlyphsRepository gdb;
  return gdb;
}

GlyphsRepository::GlyphsRepository()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "GlyphsCon"))
  , m_config(Configuration::getInstance())
  , m_assetsDir(DirManager::getInstance().assetsDir())
{
  GlyphsRepository::open();
}

void
GlyphsRepository::open()
{
  setDatabaseName(m_assetsDir.absoluteFilePath("glyphs.db"));
  if (!QSqlDatabase::open())
    qFatal("Failed to open glyphs DB, file: %s", qPrintable(m_assetsDir.absoluteFilePath("glyphs.db")));
}

DbConnection::Type
GlyphsRepository::type()
{
  return DbConnection::Glyphs;
}

QStringList
GlyphsRepository::getPageLines(const int page) const
{
  QSqlQuery dbQuery(*this);

  QString query = "SELECT %0 FROM pages WHERE page_no=%1";
  query = query.arg("qcf_v" + QString::number(m_config.qcfVersion()), QString::number(page));

  dbQuery.prepare(query);
  if (!dbQuery.exec())
    qCritical("Failed to execute SQL statment, error: %s", qPrintable(lastError().text()));

  dbQuery.next();
  QStringList lines = dbQuery.value(0).toString().trimmed().split('\n');

  return lines;
}

QString
GlyphsRepository::getSurahNameGlyph(const int sura) const
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare("SELECT qcf_v1 FROM surah_glyphs WHERE surah=:i");
  dbQuery.bindValue(0, sura);
  if (!dbQuery.exec()) {
    qCritical("Failed to execute SQL statment, error: %s", qPrintable(lastError().text()));
  }

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QString
GlyphsRepository::getJuzGlyph(const int juz) const
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare("SELECT text FROM juz_glyphs WHERE juz=:j");
  dbQuery.bindValue(0, juz);
  if (!dbQuery.exec()) {
    qFatal("Failed to execute SQL statment, error: %s", qPrintable(lastError().text()));
  }

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QString
GlyphsRepository::getVerseGlyphs(const int sIdx, const int vIdx) const
{
  QSqlQuery dbQuery(*this);

  QString query = "SELECT %0 FROM ayah_glyphs WHERE surah=%1 AND ayah=%2";
  query = query.arg("qcf_v" + QString::number(m_config.qcfVersion()), QString::number(sIdx), QString::number(vIdx));

  dbQuery.prepare(query);
  if (!dbQuery.exec())
    qFatal("Failed to execute SQL statment, error: %s", qPrintable(lastError().text()));

  dbQuery.next();

  return dbQuery.value(0).toString();
}
