#ifndef BETAQATDB_H
#define BETAQATDB_H

#include <QDir>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <utils/dirmanager.h>
#include <utils/settings.h>

class BetaqatDb
  : public DbConnection
  , QSqlDatabase
{
public:
  static QSharedPointer<BetaqatDb> current();
  BetaqatDb();
  void open();
  Type type();
  /**
   * @brief get the surah card (betaqa) content
   * @param surah - surah number
   * @return QString of the surah card text
   */
  QString getBetaqa(const int surah);

private:
  const QLocale::Language m_languageCode = Settings::language;
  const QSharedPointer<QDir> m_assetsDir = DirManager::assetsDir;
};

#endif // BETAQATDB_H
