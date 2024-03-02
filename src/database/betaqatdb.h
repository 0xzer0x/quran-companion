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
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static BetaqatDb& getInstance();
  /**
   * @brief sets and opens the sqlite database file
   */
  void open();
  /**
   * @brief getter for the type of the connection
   * @return - DbConnection::Betaqat
   */
  Type type();
  /**
   * @brief get the surah card (betaqa) content
   * @param surah - surah number
   * @return QString of the surah card text
   */
  QString getBetaqa(const int surah);

private:
  BetaqatDb();
  /**
   * @brief reference to the singleton Configuration instance
   */
  const Configuration& m_config;
  /**
   * @brief reference to the app assets directory
   */
  const QDir& m_assetsDir;
};

#endif // BETAQATDB_H
