#ifndef TRANSLATIONDB_H
#define TRANSLATIONDB_H

#include <QDir>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <types/translation.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class TranslationDb
 * @brief The TranslationDb class represents a connection to the currently
 * selected translation db
 */
class TranslationDb
  : public DbConnection
  , QSqlDatabase
{
  Q_OBJECT
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static TranslationDb& getInstance();
  /**
   * @brief sets and opens the sqlite database file
   */
  void open();
  /**
   * @brief getter for the type of the connection
   * @return - DbConnection::Translation
   */
  Type type();
  /**
   * @brief sets the active translation
   * @param translationName - DBManager::Translation entry
   */
  bool setCurrentTranslation(int idx);
  /**
   * @brief gets the translation of the given verse using the active
   * DBManager::Translation
   * @param sIdx - surah number
   * @param vIdx - verse number
   * @return QString containing the verse translation
   */
  QString getTranslation(const int sIdx, const int vIdx) const;
  /**
   * @brief getter for m_currTr
   * @return pointer to the currently selected translation
   */
  const ::Translation* currTranslation() const;

public slots:
  /**
   * @brief set translation to the one in the settings, update the selected db
   */
  void updateLoadedTranslation();

private:
  TranslationDb();
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;
  /**
   * @brief reference to the singleton DirManager instance
   */
  const DirManager& m_dirMgr;
  /**
   * @brief reference to the static QList of available translations
   */
  const QList<::Translation>& m_translations;
  /**
   * @brief the current active DBManager::Translation
   */
  const ::Translation* m_currTr;
  /**
   * @brief path to the currently active translation database file
   */
  QFileInfo m_translationFile;
};

#endif // TRANSLATIONDB_H
