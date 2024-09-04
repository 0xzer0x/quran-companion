#ifndef BETAQATREPOSITORY_H
#define BETAQATREPOSITORY_H

#include <QDir>
#include <QSqlDatabase>
#include <repository/dbconnection.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class BetaqatRepository
 * @brief Manages the connection to the betaqat database file and provides
 * methods for querying.
 *
 * This class is a singleton that handles database operations related to the
 * betaqat database. It extends `DbConnection` and `QSqlDatabase` to manage
 * database connections and queries.
 */
class BetaqatRepository
  : public DbConnection
  , private QSqlDatabase
{
public:
  /**
   * @brief Provides access to the singleton instance of BetaqatRepository.
   * @return Reference to the static class instance.
   */
  static BetaqatRepository& getInstance();
  /**
   * @brief Opens the connection to the betaqat database.
   */
  void open() override;
  /**
   * @brief Returns the type of database connection.
   * @return Type of the database connection as DbConnection::Betaqat.
   */
  Type type() override;
  /**
   * @brief Retrieves the betaqa text for a specific surah.
   * @param surah The surah number to query.
   * @return QString containing the betaqa text.
   */
  QString getBetaqa(const int surah) const;

private:
  BetaqatRepository();
  /**
   * @brief Reference to the singleton Configuration instance.
   */
  const Configuration& m_config;
  /**
   * @brief Reference to the application assets directory.
   */
  const QDir& m_assetsDir;
};

#endif // BETAQATREPOSITORY_H
