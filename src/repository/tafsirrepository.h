#ifndef TAFSIRREPOSITORY_H
#define TAFSIRREPOSITORY_H

#include <QDir>
#include <QPointer>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <repository/dbconnection.h>
#include <types/tafsir.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class TafsirRepository
 * @brief The TafsirRepository class represents a connection to the currently
 * selected tafsir db
 */
class TafsirRepository
  : public DbConnection
  , QSqlDatabase

{
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static TafsirRepository& getInstance();

  void open() override;

  Type type() override;

  void loadTafsir();

  bool setCurrentTafsir(int idx);

  QString getTafsir(const int sIdx, const int vIdx);

  const ::Tafsir* currTafsir() const;

private:
  TafsirRepository();
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

#endif // TAFSIRREPOSITORY_H
