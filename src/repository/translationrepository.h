#ifndef TRANSLATIONREPOSITORY_H
#define TRANSLATIONREPOSITORY_H

#include <QDir>
#include <QPointer>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <repository/dbconnection.h>
#include <types/translation.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class TranslationRepository
 * @brief The TranslationRepository class provides access to the translation
 * database.
 *
 * This class is responsible for managing the connection to the translation
 * database, loading the current translation, and retrieving translation text
 * for specific surahs and ayahs. It follows the Singleton design pattern to
 * ensure a single instance throughout the application.
 */
class TranslationRepository
  : public DbConnection
  , public QSqlDatabase
{
public:
  /**
   * @brief Gets the singleton instance of the TranslationRepository.
   * @return Reference to the static instance of the class.
   */
  static TranslationRepository& getInstance();
  /**
   * @brief Opens the database connection.
   *
   * This method sets the database name and attempts to open the connection.
   * If the connection fails, it will terminate the application with a fatal
   * error.
   */
  void open() override;
  /**
   * @brief Returns the type of database connection.
   * @return The type of database connection as an enum value from DbConnection.
   */
  Type type() override;
  /**
   * @brief Loads the current translation based on configuration settings.
   *
   * This method retrieves the currently selected translation from the
   * configuration settings and updates the internal state of the repository.
   */
  void loadTranslation();
  /**
   * @brief Sets the current translation to the specified ID.
   * @param id The ID of the translation to set as the current translation.
   * @return True if the translation was successfully set, false otherwise.
   */
  bool setCurrentTranslation(QString id);
  /**
   * @brief Retrieves the translation text for a specific surah and ayah.
   * @param sIdx Index of the surah.
   * @param vIdx Index of the ayah.
   * @return The translation text for the specified surah and ayah.
   */
  QString getTranslation(const int sIdx, const int vIdx) const;
  /**
   * @brief Gets the currently selected translation.
   * @return An optional containing the current translation, or an empty
   * optional if none is set.
   */
  std::optional<const ::Translation> currTranslation() const;

private:
  /**
   * @brief Private constructor for singleton pattern.
   */
  TranslationRepository();
  /**
   * @brief Reference to the singleton Configuration instance.
   */
  Configuration& m_config;
  /**
   * @brief Reference to the singleton DirManager instance.
   */
  const DirManager& m_dirMgr;
  /**
   * @brief Reference to the static QList of available translations.
   */
  const QList<::Translation>& m_translations;
  /**
   * @brief Pointer to the currently selected Translation.
   */
  std::optional<::Translation> m_currTranslation;
  /**
   * @brief Path to the currently active translation database file.
   */
  QFileInfo m_translationFile;
};

#endif // TRANSLATIONREPOSITORY_H
