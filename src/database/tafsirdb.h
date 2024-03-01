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

class TafsirDb
  : public DbConnection
  , QSqlDatabase

{
public:
  static TafsirDb& getInstance();
  void open();
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
   * @return the currently set DBManager::Tafasir
   */
  const ::Tafsir* currTafsir() const;

private:
  TafsirDb();
  Configuration& m_config;
  const DirManager& m_dirMgr;
  const QList<::Tafsir>& m_tafasir;
  /**
   * @brief the current active DBManager::Tafasir
   */
  const ::Tafsir* m_currTafsir;
  /**
   * @brief path to the currently active tafsir database file
   */
  QFileInfo m_tafsirFile;
};

#endif // TAFSIRDB_H
