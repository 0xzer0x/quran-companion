#ifndef TAFSIRDB_H
#define TAFSIRDB_H

#include <QDir>
#include <QPointer>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <types/tafsir.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class TafsirDb
 * @brief The TafsirDb class represents a connection to the currently selected
 * tafsir db
 */
class TafsirDb
  : public DbConnection
  , QSqlDatabase

{
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static TafsirDb& getInstance();
  /**
   * @brief sets and opens the sqlite database file
   */
  void open();
  /**
   * @brief getter for the type of the connection
   * @return - DbConnection::Tafsir
   */
  Type type();
  /**
   * @brief set tafsir to the one in the settings, update the selected db
   */
  void updateLoadedTafsir();
  /**
   * @brief sets the active tafsir
   * @param tafsirName - DBManager::Tafsir entry
   */
  bool setCurrentTafsir(int idx);
  /**
   * @brief gets the tafsir content for the given verse using the active
   * DBManager::Tafsir
   * @param sIdx - surah number
   * @param vIdx - verse number
   * @return QString containing the tafsir of the verse
   */
  QString getTafsir(const int sIdx, const int vIdx);
  /**
   * @brief getter for m_currTafsir
   * @return pointer to the currently selected Tafasir
   */
  const ::Tafsir* currTafsir() const;

private:
  TafsirDb();
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;
  /**
   * @brief reference to the singleton DirManager instance
   */
  const DirManager& m_dirMgr;
  /**
   * @brief reference to the static QList of available tafasir
   */
  const QList<::Tafsir>& m_tafasir;
  /**
   * @brief pointer to the currently selected Tafsir
   */
  const ::Tafsir* m_currTafsir;
  /**
   * @brief path to the currently active tafsir database file
   */
  QFileInfo m_tafsirFile;
};

#endif // TAFSIRDB_H
