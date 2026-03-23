#include "tafsirrepository.h"
#include <QSqlQuery>
#include <types/tafsir.h>

TafsirRepository&
TafsirRepository::getInstance()
{
  static TafsirRepository tadb;
  return tadb;
}

TafsirRepository::TafsirRepository()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "TafsirCon"))
  , m_config(Configuration::getInstance())
  , m_dirMgr(DirManager::getInstance())
  , m_currTafsir(m_config.tafsir())
{
  loadTafsir();
}

void
TafsirRepository::open()
{
  setDatabaseName(m_tafsirFile.absoluteFilePath());
  if (!QSqlDatabase::open())
    qFatal("Failed to open tafsir DB, file: %s", qPrintable(m_tafsirFile.absoluteFilePath()));
}

DbConnection::Type
TafsirRepository::type()
{
  return DbConnection::Tafsir;
}

void
TafsirRepository::loadTafsir()
{
  setCurrentTafsir(m_config.tafsir());
}

bool
TafsirRepository::setCurrentTafsir(const ::Tafsir tafsir)
{
  const QDir& baseDir = tafsir.isExtra() ? m_dirMgr.downloadsDir() : m_dirMgr.assetsDir();
  QString path = "tafasir/" + tafsir.filename();
  if (!baseDir.exists(path))
    return false;

  m_currTafsir = tafsir;
  m_tafsirFile.setFile(baseDir.filePath(path));
  TafsirRepository::open();

  return true;
}

QString
TafsirRepository::getTafsir(const int sIdx, const int vIdx)
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

const Tafsir
TafsirRepository::currTafsir() const
{
  return m_currTafsir;
}
