#ifndef BETAQATDB_H
#define BETAQATDB_H

#include <QDir>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

class BetaqatDb
  : public DbConnection
  , QSqlDatabase
{
public:
  static BetaqatDb& getInstance();
  void open();
  Type type();
  /**
   * @brief get the surah card (betaqa) content
   * @param surah - surah number
   * @return QString of the surah card text
   */
  QString getBetaqa(const int surah);

private:
  BetaqatDb();
  const Configuration& m_config;
  const QDir& m_assetsDir;
};

#endif // BETAQATDB_H
