#ifndef TRANSLATIONDB_H
#define TRANSLATIONDB_H

#include <QDir>
#include <QObject>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <interfaces/dbconnection.h>
#include <types/translation.h>
#include <utils/dirmanager.h>
#include <utils/settings.h>

class TranslationDb
  : public DbConnection
  , QSqlDatabase
{
  Q_OBJECT
public:
  static QSharedPointer<TranslationDb> current();
  TranslationDb();
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
  QString getTranslation(const int sIdx, const int vIdx);
  /**
   * @brief currTranslation
   * @return
   *
   * MODIFIED
   */
  QSharedPointer<::Translation> currTranslation() const;

public slots:
  /**
   * @brief set translation to the one in the settings, update the selected db
   */
  void updateLoadedTranslation();

private:
  const QSharedPointer<QDir> m_assetsDir = DirManager::assetsDir;
  const QSharedPointer<QDir> m_downloadsDir = DirManager::downloadsDir;
  const QSharedPointer<QSettings> m_settings = Settings::settings;
  const QList<QSharedPointer<::Translation>>& m_translations =
    Translation::translations;
  /**
   * @brief the current active DBManager::Translation
   */
  QSharedPointer<::Translation> m_currTr;
  /**
   * @brief path to the currently active translation database file
   */
  QFileInfo m_translationFile;
};

#endif // TRANSLATIONDB_H
