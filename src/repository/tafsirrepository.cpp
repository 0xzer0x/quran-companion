#include "tafsirrepository.h"
#include <QSqlQuery>

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
  , m_tafasir(Tafsir::tafasir)
{
  loadTafsir();
}

void
TafsirRepository::open()
{
  setDatabaseName(m_tafsirFile.absoluteFilePath());
  if (!QSqlDatabase::open())
    qFatal("Error opening tafsir db");
}

DbConnection::Type
TafsirRepository::type()
{
  return DbConnection::Tafsir;
}

void
TafsirRepository::loadTafsir()
{
  int curr = m_config.settings().value("Reader/Tafsir").toInt();
  setCurrentTafsir(curr);
}

bool
TafsirRepository::setCurrentTafsir(int idx)
{
  if (idx < 0 || idx >= m_tafasir.size())
    return false;
  if (m_currTafsir == &m_tafasir[idx])
    return true;

  m_currTafsir = &m_tafasir[idx];
  const QDir& baseDir =
    m_currTafsir->isExtra() ? m_dirMgr.downloadsDir() : m_dirMgr.assetsDir();
  QString path = "tafasir/" + m_currTafsir->filename();
  if (!baseDir.exists(path))
    return false;

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

const Tafsir*
TafsirRepository::currTafsir() const
{
  return m_currTafsir;
}
