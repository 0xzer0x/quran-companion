#include "glyphsdb.h"
#include <QSqlQuery>

QSharedPointer<GlyphsDb>
GlyphsDb::current()
{
  static QSharedPointer<GlyphsDb> glyphsDb = QSharedPointer<GlyphsDb>::create();
  return glyphsDb;
}

GlyphsDb::GlyphsDb()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "GlyphsCon"))
{
  GlyphsDb::open();
}

void
GlyphsDb::open()
{
  setDatabaseName(m_assetsDir->absoluteFilePath("glyphs.db"));
  if (!QSqlDatabase::open())
    qFatal("Error opening glyphs db");
}

DbConnection::Type
GlyphsDb::type()
{
  return DbConnection::Glyphs;
}

QStringList
GlyphsDb::getPageLines(const int page)
{
  QSqlQuery dbQuery(*this);

  QString query = "SELECT %0 FROM pages WHERE page_no=%1";
  query = query.arg("qcf_v" + QString::number(m_qcfVer), QString::number(page));

  dbQuery.prepare(query);
  if (!dbQuery.exec())
    qFatal("Couldn't execute getPageLines query!");

  dbQuery.next();
  QStringList lines = dbQuery.value(0).toString().trimmed().split('\n');

  return lines;
}

QString
GlyphsDb::getSurahNameGlyph(const int sura)
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
GlyphsDb::getJuzGlyph(const int juz)
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
GlyphsDb::getVerseGlyphs(const int sIdx, const int vIdx)
{
  QSqlQuery dbQuery(*this);

  QString query = "SELECT %0 FROM ayah_glyphs WHERE surah=%1 AND ayah=%2";
  query = query.arg("qcf_v" + QString::number(m_qcfVer),
                    QString::number(sIdx),
                    QString::number(vIdx));

  dbQuery.prepare(query);
  if (!dbQuery.exec())
    qFatal("Couldn't execute getVerseGlyphs query!");

  dbQuery.next();

  return dbQuery.value(0).toString();
}
