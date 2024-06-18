#ifndef TRANSLATIONREPOSITORY_H
#define TRANSLATIONREPOSITORY_H

#include <QDir>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QSqlDatabase>
#include <repository/dbconnection.h>
#include <types/translation.h>
#include <utils/configuration.h>
#include <utils/dirmanager.h>

/**
 * @class TranslationRepository
 * @brief The TranslationRepository class represents a connection to the
 * currently selected translation db
 */
class TranslationRepository
  : public DbConnection
  , QSqlDatabase
{
  Q_OBJECT
public:
  /**
   * @brief get a reference to the single class instance
   * @return reference to the static class instance
   */
  static TranslationRepository& getInstance();

  void open() override;

  Type type() override;

  void loadTranslation();

  bool setCurrentTranslation(int idx);

  QString getTranslation(const int sIdx, const int vIdx) const;

  const ::Translation* currTranslation() const;

private:
  TranslationRepository();
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

#endif // TRANSLATIONREPOSITORY_H
