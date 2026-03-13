#include "translationrepository.h"
#include <QSqlQuery>

TranslationRepository&
TranslationRepository::getInstance()
{
  static TranslationRepository tdb;
  return tdb;
}

TranslationRepository::TranslationRepository()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "TranslationCon"))
  , m_dirMgr(DirManager::getInstance())
  , m_config(Configuration::getInstance())
  , m_currTranslation(m_config.translation())
{
}

void
TranslationRepository::open()
{
  setDatabaseName(m_translationFile.absoluteFilePath());
  if (!QSqlDatabase::open())
    qFatal("Error opening translation db");
}

DbConnection::Type
TranslationRepository::type()
{
  return DbConnection::Translation;
}

void
TranslationRepository::loadTranslation()
{
  setCurrentTranslation(m_config.translation());
}

bool
TranslationRepository::setCurrentTranslation(const ::Translation translation)
{
  const QDir& baseDir = translation.isExtra()
                          ? DirManager::getInstance().downloadsDir()
                          : DirManager::getInstance().assetsDir();
  QString path = "translations/" + translation.filename();
  if (!baseDir.exists(path))
    return false;

  m_currTranslation = translation;
  m_translationFile.setFile(baseDir.filePath(path));
  TranslationRepository::open();
  return true;
}

QString
TranslationRepository::getTranslation(const int sIdx, const int vIdx) const
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare("SELECT text FROM content WHERE sura=:s AND aya=:v");
  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec())
    qCritical("Couldn't execute getTranslation query!");

  dbQuery.next();

  return dbQuery.value(0).toString();
}

const ::Translation
TranslationRepository::currTranslation() const
{
  return m_currTranslation;
}
