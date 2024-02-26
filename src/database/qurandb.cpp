#include "qurandb.h"
#include <QRandomGenerator>
#include <QSqlError>

QSharedPointer<QuranDb>
QuranDb::current()
{
  static QSharedPointer<QuranDb> quranDb = QSharedPointer<QuranDb>::create();
  return quranDb;
}

QuranDb::QuranDb()
  : QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "QuranCon"))
{
  QuranDb::open();
  for (int i = 1; i <= 114; i++)
    m_surahNames.append(surahName(i));
}

void
QuranDb::open()
{
  setDatabaseName(m_assetsDir->absoluteFilePath("quran.db"));
  if (!QSqlDatabase::open())
    qFatal("Error opening quran db");
}

DbConnection::Type
QuranDb::type()
{
  return DbConnection::Quran;
}

QPair<int, int>
QuranDb::pageMetadata(const int page)
{
  QSqlQuery dbQuery(*this);
  dbQuery.prepare(
    "SELECT sura_no,jozz FROM verses_v1 WHERE page=:p ORDER BY id");
  dbQuery.bindValue(0, page);

  if (!dbQuery.exec())
    qCritical() << "Error occurred during getPageMetadata SQL statment exec";

  dbQuery.next();
  // { surahIdx, jozz }
  return { dbQuery.value(0).toInt(), dbQuery.value(1).toInt() };
}

int
QuranDb::getVersePage(const int& surahIdx, const int& verse)
{
  QSqlQuery dbQuery(*this);

  QString query = "SELECT page FROM verses_v%0 WHERE sura_no=%1 AND aya_no=%2";
  dbQuery.prepare(query.arg(QString::number(m_qcfVer),
                            QString::number(surahIdx),
                            QString::number(verse)));

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getVersePage SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toInt();
}

int
QuranDb::getJuzStartPage(const int juz)
{
  QSqlQuery dbQuery(*this);

  QString query =
    "SELECT page FROM verses_v1 WHERE jozz=" + QString::number(juz) +
    " ORDER BY id";
  dbQuery.prepare(query);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getJuzStartPage SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toInt();
}

int
QuranDb::getJuzOfPage(const int page)
{
  QSqlQuery dbQuery(*this);

  QString query =
    "SELECT jozz FROM verses_v1 WHERE page=" + QString::number(page);
  dbQuery.prepare(query);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getJuzOfPage SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toInt();
}

QList<QList<int>>
QuranDb::verseInfoList(const int page)
{
  QList<QList<int>> viList;
  QSqlQuery dbQuery(*this);

  QString query =
    "SELECT sura_no,aya_no FROM verses_v%0 WHERE page=%1 ORDER BY id";
  dbQuery.prepare(query.arg(QString::number(m_qcfVer), QString::number(page)));

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getVerseInfoList SQL statment exec";
  }

  while (dbQuery.next()) {
    QList<int> v{ page, dbQuery.value(0).toInt(), dbQuery.value(1).toInt() };
    viList.append(v);
  }

  return viList;
}

QString
QuranDb::verseText(const int sIdx, const int vIdx)
{
  QSqlQuery dbQuery(*this);
  if (m_verseType == VerseType::Annotated)
    dbQuery.prepare("SELECT aya_text_annotated FROM verses_v1 WHERE sura_no=:s "
                    "AND aya_no=:v");
  else
    dbQuery.prepare(
      "SELECT aya_text FROM verses_v1 WHERE sura_no=:s AND aya_no=:v");

  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getVerseText SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toString();
}

int
QuranDb::surahStartPage(int surahIdx)
{
  QSqlQuery dbQuery(*this);

  dbQuery.prepare("SELECT page FROM verses_v1 WHERE sura_no=:sn AND aya_no=1");
  dbQuery.bindValue(0, surahIdx);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getSurahStartPage SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toInt();
}

QString
QuranDb::surahName(const int sIdx, bool ar)
{
  QSqlQuery dbQuery(*this);

  if (m_languageCode == QLocale::Arabic || ar)
    dbQuery.prepare("SELECT sura_name_ar FROM verses_v1 WHERE sura_no=:i");
  else
    dbQuery.prepare("SELECT sura_name_en FROM verses_v1 WHERE sura_no=:i");

  dbQuery.bindValue(0, sIdx);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getSurahName SQL statment exec";
  }

  dbQuery.next();
  return dbQuery.value(0).toString();
}

int
QuranDb::verseId(const int sIdx, const int vIdx)
{
  QSqlQuery dbQuery(*this);
  dbQuery.prepare("SELECT id FROM verses_v1 WHERE sura_no=:s AND aya_no=:v");
  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getVerseId SQL statment exec";
  }

  dbQuery.next();
  return dbQuery.value(0).toInt();
}

QList<int>
QuranDb::verseById(const int id)
{
  QSqlQuery dbQuery(*this);
  dbQuery.prepare("SELECT page,sura_no,aya_no FROM verses_v1 WHERE id=:i");
  dbQuery.bindValue(0, id);

  if (!dbQuery.exec())
    qCritical() << "Error occurred during getVerseById SQL statement exec";

  dbQuery.next();

  return { dbQuery.value(0).toInt(),
           dbQuery.value(1).toInt(),
           dbQuery.value(2).toInt() };
}

int
QuranDb::versePage(const int& surahIdx, const int& verse)
{
  QSqlQuery dbQuery(*this);

  QString query = "SELECT page FROM verses_v%0 WHERE sura_no=%1 AND aya_no=%2";
  dbQuery.prepare(query.arg(QString::number(m_qcfVer),
                            QString::number(surahIdx),
                            QString::number(verse)));

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getVersePage SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toInt();
}

QList<int>
QuranDb::searchSurahNames(QString text)
{
  QList<int> results;
  QSqlQuery dbQuery(*this);
  QString q =
    "SELECT DISTINCT sura_no FROM verses_v1 WHERE (sura_name_ar like '%" +
    text +
    "%' OR "
    "sura_name_en like '%" +
    text + "%')";

  dbQuery.prepare(q);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during searchSurahNames SQL statment exec";
  }

  while (dbQuery.next()) {
    results.append(dbQuery.value(0).toInt());
  }

  return results;
}

QList<QList<int>>
QuranDb::searchSurahs(QString searchText,
                      const QList<int> surahs,
                      const bool whole)
{
  QList<QList<int>> results;
  QSqlQuery dbQuery(*this);

  QString q = "SELECT page,sura_no,aya_no FROM verses_v" +
              QString::number(m_qcfVer) + " WHERE (";
  for (int i = 0; i < surahs.size(); i++) {
    q.append("sura_no=" + QString::number(surahs.at(i)) + ' ');
    if (i != surahs.size() - 1)
      q.append("OR ");
  }

  if (whole)
    q.append(") AND (aya_text_emlaey like '" + searchText +
             " %' OR aya_text_emlaey like '% " + searchText +
             " %') ORDER BY id");
  else
    q.append(") AND (aya_text_emlaey like '%" + searchText + "%') ORDER BY id");

  dbQuery.prepare(q);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during searchSurahs SQL statment exec";
  }

  while (dbQuery.next()) {
    results.append({ dbQuery.value(0).toInt(),
                     dbQuery.value(1).toInt(),
                     dbQuery.value(2).toInt() });
  }

  return results;
}

QList<QList<int>>
QuranDb::searchVerses(QString searchText, const int range[], const bool whole)
{
  QList<QList<int>> results;
  QSqlQuery dbQuery(*this);

  QString q = "SELECT page,sura_no,aya_no FROM verses_v" +
              QString::number(m_qcfVer) +
              " WHERE (page >= " + QString::number(range[0]) +
              " AND page <= " + QString::number(range[1]) + ")";

  if (whole)
    q.append(" AND (aya_text_emlaey like '" + searchText +
             " %' OR aya_text_emlaey like '% " + searchText +
             " %') ORDER BY id");
  else
    q.append(" AND (aya_text_emlaey like '%" + searchText + "%') ORDER BY id");

  dbQuery.prepare(q);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during searchVerses SQL statment exec";
  }

  while (dbQuery.next()) {
    QList<int> entry{ dbQuery.value(0).toInt(),
                      dbQuery.value(1).toInt(),
                      dbQuery.value(2).toInt() };
    results.append(entry);
  }

  return results;
}

QList<int>
QuranDb::randomVerse()
{
  QSqlQuery dbQuery(*this);

  int id = QRandomGenerator::global()->bounded(1, 6237);
  dbQuery.prepare("SELECT page,sura_no,aya_no FROM verses_v" +
                  QString::number(m_qcfVer) +
                  " WHERE id=" + QString::number(id));

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during randomVerse SQL statment exec";
  }
  dbQuery.next();
  return { dbQuery.value(0).toInt(),
           dbQuery.value(1).toInt(),
           dbQuery.value(2).toInt() };
}

void
QuranDb::setVerseType(VerseType newVerseType)
{
  m_verseType = newVerseType;
}

VerseType
QuranDb::verseType() const
{
  return m_verseType;
}

QStringList
QuranDb::surahNames() const
{
  return m_surahNames;
}
