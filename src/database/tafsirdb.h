#ifndef TAFSIRDB_H
#define TAFSIRDB_H

#include <QDir>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <types/tafsir.h>
#include <utils/dirmanager.h>
#include <utils/settings.h>

class TafsirDb
  : public DbConnection
  , QSqlDatabase

{
public:
    static QSharedPointer<TafsirDb> current();
  TafsirDb();
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
  QSharedPointer<::Tafsir> currTafsir() const;

private:
  const QSharedPointer<QSettings> m_settings = Settings::settings;
  const QSharedPointer<QDir> m_assetsDir = DirManager::assetsDir;
  const QSharedPointer<QDir> m_downloadsDir = DirManager::downloadsDir;
  const QList<QSharedPointer<::Tafsir>>& m_tafasir = Tafsir::tafasir;
  /**
   * @brief the current active DBManager::Tafasir
   */
  QSharedPointer<::Tafsir> m_currTafsir;
  /**
   * @brief path to the currently active tafsir database file
   */
  QFileInfo m_tafsirFile;
};

#endif // TAFSIRDB_H
