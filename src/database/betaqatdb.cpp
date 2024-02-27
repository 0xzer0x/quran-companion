#include "betaqatdb.h"
#include <QSqlQuery>

QSharedPointer<BetaqatDb>
BetaqatDb::current()
{
  static QSharedPointer<BetaqatDb> betaqatDb =
    QSharedPointer<BetaqatDb>::create();
  return betaqatDb;
}

BetaqatDb::BetaqatDb()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "BetaqatCon"))
{
  BetaqatDb::open();
}

void
BetaqatDb::open()
{
  setDatabaseName(m_assetsDir->absoluteFilePath("betaqat.db"));
  if (!QSqlDatabase::open())
    qFatal("Error opening betaqat db");
}

DbConnection::Type
BetaqatDb::type()
{
  return DbConnection::Betaqat;
}

QString
BetaqatDb::getBetaqa(const int surah)
{
  QSqlQuery dbQuery(*this);

  if (m_languageCode == QLocale::Arabic)
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