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

class TranslationDb
  : public DbConnection
  , QSqlDatabase
{
  Q_OBJECT
public:
  static TranslationDb& getInstance();
  void open();
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
   * @brief currTranslation
   * @return
   *
   * MODIFIED
   */
  const ::Translation* currTranslation() const;

public slots:
  /**
   * @brief set translation to the one in the settings, update the selected db
   */
  void updateLoadedTranslation();

private:
  TranslationDb();
  Configuration& m_config;
  const DirManager& m_dirMgr;
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
