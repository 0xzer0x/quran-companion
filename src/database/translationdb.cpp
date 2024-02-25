#include "translationdb.h"
#include <QSqlQuery>

QSharedPointer<TranslationDb>
TranslationDb::current()
{
  static QSharedPointer<TranslationDb> translationDb =
    QSharedPointer<TranslationDb>::create();
  return translationDb;
}

TranslationDb::TranslationDb()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "TranslationCon"))
{
}

void
TranslationDb::open()
{
  setDatabaseName(m_translationFile.absoluteFilePath());
  if (!QSqlDatabase::open())
    qFatal("Error opening translation db");
}

DbConnection::Type
TranslationDb::type()
{
  return DbConnection::Translation;
}

void
TranslationDb::updateLoadedTranslation()
{
  int curr = m_settings->value("Reader/Translation").toInt();
  setCurrentTranslation(curr);
}

bool
TranslationDb::setCurrentTranslation(int idx)
{
  if (idx < 0 || idx >= m_translations.size())
    return false;
  if (m_currTr == m_translations[idx])
    return true;

  m_currTr = m_translations[idx];
  const QDir& baseDir = m_currTr->isExtra() ? *m_downloadsDir : *m_assetsDir;
  QString path = "translations/" + m_currTr->filename();
  if (!baseDir.exists(path))
    return false;

  m_translationFile.setFile(baseDir.filePath(path));
  TranslationDb::open();
  return true;
}

QString
TranslationDb::getTranslation(const int sIdx, const int vIdx)
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

QSharedPointer<Translation>
TranslationDb::currTranslation() const
{
  return m_currTr;
}
