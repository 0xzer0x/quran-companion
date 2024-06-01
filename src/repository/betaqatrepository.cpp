#include "betaqatrepository.h"
#include <QSqlQuery>

BetaqatRepository&
BetaqatRepository::getInstance()
{
  static BetaqatRepository bdb;
  return bdb;
}

BetaqatRepository::BetaqatRepository()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "BetaqatCon"))
  , m_assetsDir(DirManager::getInstance().assetsDir())
  , m_config(Configuration::getInstance())
{
  BetaqatRepository::open();
}

void
BetaqatRepository::open()
{
  setDatabaseName(m_assetsDir.absoluteFilePath("betaqat.db"));
  if (!QSqlDatabase::open())
    qFatal("Error opening betaqat db");
}

DbConnection::Type
BetaqatRepository::type()
{
  return DbConnection::Betaqat;
}

QString
BetaqatRepository::getBetaqa(const int surah) const
{
  QSqlQuery dbQuery(*this);

  if (m_config.language() == QLocale::Arabic)
    dbQuery.prepare("SELECT text FROM content WHERE sura=:i");
  else
    dbQuery.prepare("SELECT text_en FROM content WHERE sura=:i");

  dbQuery.bindValue(0, surah);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getBetaqa SQL statment exec";
  }

  dbQuery.next();
  return dbQuery.value(0).toString();
}
