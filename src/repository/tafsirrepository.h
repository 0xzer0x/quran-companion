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
 * selected tafsir database.
 *
 * This class provides methods to interact with the tafsir database, including
 * loading tafsir data, retrieving tafsir text for specific verses, and managing
 * the current tafsir selection.
 */
class TafsirRepository
  : public DbConnection
  , QSqlDatabase
{
public:
  /**
   * @brief Get a reference to the singleton instance of TafsirRepository.
   * @return Reference to the static class instance.
   */
  static TafsirRepository& getInstance();
  /**
   * @brief Open the connection to the tafsir database.
   * This method sets up the database connection using the currently selected
   * tafsir database file.
   */
  void open() override;
  /**
   * @brief Get the type of the database connection.
   * @return The type of the database connection (Tafsir).
   */
  Type type() override;
  /**
   * @brief Load the tafsir data based on the current configuration.
   * This method retrieves the currently selected tafsir and sets up the
   * database connection accordingly.
   */
  void loadTafsir();
  /**
   * @brief Set the current tafsir based on its ID.
   * @param tafsir - The ::Tafsir to set as the current tafsir.
   * @return True if the tafsir was successfully set, false otherwise.
   */
  bool setCurrentTafsir(const ::Tafsir tafsir);
  /**
   * @brief Get the tafsir text for a specific surah and verse.
   * @param sIdx The index of the surah.
   * @param vIdx The index of the verse.
   * @return The text of the specified verse in the current tafsir.
   */
  QString getTafsir(const int sIdx, const int vIdx);
  /**
   * @brief Get the currently selected tafsir.
   * @return The currently selected ::Tafsir.
   */
  const ::Tafsir currTafsir() const;

private:
  /**
   * @brief Constructor for TafsirRepository.
   * Initializes the database connection and sets up the available tafasir list.
   */
  TafsirRepository();
  /**
   * @brief Reference to the singleton Configuration instance.
   */
  Configuration& m_config;
  /**
   * @brief Reference to the singleton DirManager instance.
   */
  const DirManager& m_dirMgr;
  /**
   * @brief The currently selected Tafsir.
   */
  ::Tafsir m_currTafsir;
  /**
   * @brief Path to the currently active tafsir database file.
   */
  QFileInfo m_tafsirFile;
};

#endif // TAFSIRREPOSITORY_H
