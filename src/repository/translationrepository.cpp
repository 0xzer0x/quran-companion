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
  , m_translations(Translation::translations)
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
  QString curr = m_config.settings().value("Reader/Translation").toString();
  bool isNum;
  curr.toInt(&isNum);
  if (isNum) {
    curr = m_translations.at(curr.toInt()).id();
    m_config.settings().setValue("Reader/Translation", curr);
  }

  setCurrentTranslation(curr);
}

bool
TranslationRepository::setCurrentTranslation(QString id)
{
  std::optional<::Translation> translation = Translation::findById(id);
  if (!translation.has_value())
    return false;

  m_currTr = translation.value();
  const QDir& baseDir = m_currTr->isExtra()
                          ? DirManager::getInstance().downloadsDir()
                          : DirManager::getInstance().assetsDir();
  QString path = "translations/" + m_currTr->filename();
  if (!baseDir.exists(path))
    return false;

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

std::optional<const ::Translation>
TranslationRepository::currTranslation() const
{
  return m_currTr;
}
