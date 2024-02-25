#include "tafsirdb.h"
#include <QSqlQuery>

QSharedPointer<TafsirDb>
TafsirDb::current()
{
  static QSharedPointer<TafsirDb> tafsirDb = QSharedPointer<TafsirDb>::create();
  return tafsirDb;
}

TafsirDb::TafsirDb()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "TafsirCon"))
{
  updateLoadedTafsir();
}

void
TafsirDb::open()
{
  setDatabaseName(m_tafsirFile.absoluteFilePath());
  if (!QSqlDatabase::open())
    qFatal("Error opening tafsir db");
}

DbConnection::Type
TafsirDb::type()
{
  return DbConnection::Tafsir;
}

void
TafsirDb::updateLoadedTafsir()
{
  int curr = m_settings->value("Reader/Tafsir").toInt();
  setCurrentTafsir(curr);
}

bool
TafsirDb::setCurrentTafsir(int idx)
{
  if (idx < 0 || idx >= m_tafasir.size())
    return false;
  if (m_currTafsir == m_tafasir[idx])
    return true;

  m_currTafsir = m_tafasir[idx];
  const QDir& baseDir =
    m_currTafsir->isExtra() ? *m_downloadsDir : *m_assetsDir;
  QString path = "tafasir/" + m_currTafsir->filename();
  if (!baseDir.exists(path))
    return false;

  m_tafsirFile.setFile(baseDir.filePath(path));
  TafsirDb::open();
  return true;
}

QString
TafsirDb::getTafsir(const int sIdx, const int vIdx)
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare("SELECT text FROM content WHERE sura=:s AND aya=:v");
  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec())
    qCritical("Couldn't execute getTafsir query!");

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QSharedPointer<Tafsir>
TafsirDb::currTafsir() const
{
  return m_currTafsir;
}
