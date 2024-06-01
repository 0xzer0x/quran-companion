#ifndef BETAQATREPOSITORY_H
#define BETAQATREPOSITORY_H

#include <QDir>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class BetaqatRepository
 * @brief The BetaqatRepository class represents a connection to the betaqat db
 * file
 */
class BetaqatRepository
  : public DbConnection
  , QSqlDatabase
{
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static BetaqatRepository& getInstance();
  void open() override;
  Type type() override;
  QString getBetaqa(const int surah) const;

private:
  BetaqatRepository();
  /**
   * @brief reference to the singleton Configuration instance
   */
  const Configuration& m_config;
  /**
   * @brief reference to the app assets directory
   */
  const QDir& m_assetsDir;
};

#endif // BETAQATREPOSITORY_H
