/**
 * @file dbmanager.cpp
 * @brief Implementation for DBManager
 */

#include "dbmanager.h"
#include <qdebug.h>

DBManager::DBManager(QObject* parent)
  : QObject(parent)
{
  m_quranDbPath.setFile(m_dbDir.filePath("quran.db"));
  m_glyphsDbPath.setFile(m_dbDir.filePath("glyphs.db"));
  m_betaqatDbPath.setFile(m_dbDir.filePath("betaqat.db"));

  // set database driver, set the path & open a connection with the db
  QSqlDatabase::addDatabase("QSQLITE", "QuranCon");
  QSqlDatabase::addDatabase("QSQLITE", "GlyphsCon");
  QSqlDatabase::addDatabase("QSQLITE", "BetaqatCon");
  QSqlDatabase::addDatabase("QSQLITE", "BookmarksCon");
  QSqlDatabase::addDatabase("QSQLITE", "TafsirCon");
  QSqlDatabase::addDatabase("QSQLITE", "TranslationCon");

  for (int i = 1; i <= 114; i++) {
    m_surahNames.append(getSurahName(i));
  }
}

/* ---------------- Database handling ---------------- */

void
DBManager::setOpenDatabase(Database db, QString filePath)
{
  if (m_currentDb == db)
    return;

  m_currentDb = db;
  m_openDBCon.close();
  switch (db) {
    case null:
      break;

    case quran:
      m_openDBCon = QSqlDatabase::database("QuranCon");
      break;

    case glyphs:
      m_openDBCon = QSqlDatabase::database("GlyphsCon");
      break;

    case bookmarks:
      m_openDBCon = QSqlDatabase::database("BookmarksCon");
      break;

    case tafsir:
      m_openDBCon = QSqlDatabase::database("TafsirCon");
      break;

    case translation:
      m_openDBCon = QSqlDatabase::database("TranslationCon");
      break;

    case betaqat:
      m_openDBCon = QSqlDatabase::database("BetaqatCon");
      break;
  }

  m_openDBCon.setDatabaseName(filePath);
  if (!m_openDBCon.open())
    qFatal("Couldn't open Database!");
}

void
DBManager::setCurrentTafsir(int tafsirIdx)
{
  if (tafsirIdx < 0 || tafsirIdx >= m_tafasirList.size())
    return;

  m_dbDir.cd("tafasir");
  m_currTafsir = &m_tafasirList[tafsirIdx];
  m_tafsirDbPath.setFile(m_dbDir.filePath(m_currTafsir->filename));
  m_dbDir.cdUp();
}

void
DBManager::setCurrentTranslation(int translationIdx)
{
  if (translationIdx < 0 || translationIdx >= m_translationsList.size())
    return;

  m_dbDir.cd("translations");
  m_currTrans = &m_translationsList[translationIdx];
  m_transDbPath.setFile(m_dbDir.filePath(m_currTrans->filename));
  m_dbDir.cdUp();
}

/* ---------------- Page-related methods ---------------- */

QPair<int, int>
DBManager::getPageMetadata(const int page)
{
  QList<int> data; // { surahIdx, jozz }
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.prepare(
    "SELECT sura_no,jozz FROM verses_v1 WHERE page=:p ORDER BY id");
  dbQuery.bindValue(0, page);

  if (!dbQuery.exec())
    qCritical() << "Error occurred during getPageMetadata SQL statment exec";

  dbQuery.next();
  return { dbQuery.value(0).toInt(), dbQuery.value(1).toInt() };
}

QStringList
DBManager::getPageLines(const int page)
{
  setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  QString query = "SELECT %0 FROM pages WHERE page_no=%1";
  query = query.arg("qcf_v" + QString::number(m_qcfVer), QString::number(page));

  dbQuery.prepare(query);
  if (!dbQuery.exec())
    qFatal("Couldn't execute getPageLines query!");

  dbQuery.next();
  QStringList lines = dbQuery.value(0).toString().trimmed().split('\n');

  return lines;
}

QList<Verse>
DBManager::getVerseInfoList(int page)
{
  QList<Verse> viList;
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  QString query =
    "SELECT sura_no,aya_no FROM verses_v%0 WHERE page=%1 ORDER BY id";
  dbQuery.prepare(query.arg(QString::number(m_qcfVer), QString::number(page)));

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getVerseInfoList SQL statment exec";
  }

  while (dbQuery.next()) {
    Verse v{ page, dbQuery.value(0).toInt(), dbQuery.value(1).toInt() };
    viList.append(v);
  }

  return viList;
}

int
DBManager::getJuzStartPage(const int juz)
{
  setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  QString query =
    "SELECT page FROM juz_glyphs WHERE juz=" + QString::number(juz);
  dbQuery.prepare(query);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getJuzStartPage SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toInt();
}

int
DBManager::getJuzOfPage(const int page)
{
  // returns the jozz number which the passed page belongs to
  setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  QString query =
    "SELECT juz FROM juz_glyphs WHERE page=" + QString::number(page);
  dbQuery.prepare(query);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getJuzOfPage SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toInt();
}

/* ---------------- Glyph-related methods ---------------- */

QString
DBManager::getSurahNameGlyph(const int sura)
{
  setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare("SELECT qcf_v1 FROM surah_glyphs WHERE surah=:i");
  dbQuery.bindValue(0, sura);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getSurahNameGlyph SQL statment exec";
  }

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QString
DBManager::getJuzGlyph(const int juz)
{
  setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare("SELECT text FROM juz_glyphs WHERE juz=:j");
  dbQuery.bindValue(0, juz);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getJuzGlyph SQL statment exec";
  }

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QString
DBManager::getVerseGlyphs(const int sIdx, const int vIdx)
{
    if (m_verseType != VerseType::qcf)
    return getVerseText(sIdx, vIdx);

  setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());

  QSqlQuery dbQuery(m_openDBCon);

  QString query = "SELECT %0 FROM ayah_glyphs WHERE surah=%1 AND ayah=%2";
  query = query.arg("qcf_v" + QString::number(m_qcfVer),
                    QString::number(sIdx),
                    QString::number(vIdx));

  dbQuery.prepare(query);
  if (!dbQuery.exec())
    qFatal("Couldn't execute getVerseGlyphs query!");

  dbQuery.next();

  return dbQuery.value(0).toString();
}

/* ---------------- Surah-related methods ---------------- */

QString
DBManager::getSurahName(const int sIdx, bool ar)
{
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

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

QString
DBManager::getBetaqa(const int surah)
{
  setOpenDatabase(betaqat, m_betaqatDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  if (m_languageCode == QLocale::Arabic)
    dbQuery.prepare("SELECT text FROM content WHERE sura=:i");
  else
    dbQuery.prepare("SELECT text_en FROM content WHERE sura=:i");

  dbQuery.bindValue(0, surah);
  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getBetaqa SQL statment exec";
  }

  dbQuery.next();
  return dbQuery.value(0).toString();
}

int
DBManager::getVerseId(const int sIdx, const int vIdx)
{
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.prepare("SELECT id FROM verses_v1 WHERE sura_no=:s AND aya_no=:v");
  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getVerseId SQL statment exec";
  }

  dbQuery.next();
  return dbQuery.value(0).toInt();
}

Verse
DBManager::getVerseById(const int id)
{
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.prepare("SELECT page,sura_no,aya_no FROM verses_v1 WHERE id=:i");
  dbQuery.bindValue(0, id);

  if (!dbQuery.exec())
    qCritical() << "Error occurred during getVerseById SQL statement exec";

  dbQuery.next();

  return Verse{ dbQuery.value(0).toInt(),
                dbQuery.value(1).toInt(),
                dbQuery.value(2).toInt() };
}

int
DBManager::getSurahVerseCount(const int surahIdx)
{
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare(
    "SELECT aya_no FROM verses_v1 WHERE sura_no=:idx ORDER BY aya_no DESC");
  dbQuery.bindValue(0, surahIdx);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getSurahVerseCount SQL statment exec";
    return -1;
  }

  dbQuery.next();
  return dbQuery.value(0).toInt();
}

int
DBManager::getSurahStartPage(int surahIdx)
{
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare("SELECT page FROM verses_v1 WHERE sura_no=:sn AND aya_no=1");
  dbQuery.bindValue(0, surahIdx);

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during getSurahStartPage SQL statment exec";
  }
  dbQuery.next();

  return dbQuery.value(0).toInt();
}

QList<QString>
DBManager::surahNameList()
{

  return m_surahNames;
}

QList<Verse>
DBManager::searchSurahs(QString searchText,
                        const QList<int> surahs,
                        const bool whole)
{
  QList<Verse> results;
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

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
    results.append(Verse{ dbQuery.value(0).toInt(),
                          dbQuery.value(1).toInt(),
                          dbQuery.value(2).toInt() });
  }

  return results;
}

QList<int>
DBManager::searchSurahNames(QString text)
{
  QList<int> results;
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);
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

/* ---------------- Verse-related methods ---------------- */

bool
DBManager::getKhatmahPos(const int khatmahId, Verse& v)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);

  QString q = QString::asprintf(
    "SELECT page,surah,number FROM khatmah WHERE id=%i", khatmahId);
  if (!dbQuery.exec(q)) {
    qCritical() << "Couldn't execute getPosition SQL query!";
    return false;
  }
  if (!dbQuery.next())
    return false;

  v.page = dbQuery.value(0).toInt();
  v.surah = dbQuery.value(1).toInt();
  v.number = dbQuery.value(2).toInt();
  return true;
}

int
DBManager::addKhatmah(const Verse& v, const QString name, const int id)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.exec(
    "CREATE TABLE IF NOT EXISTS khatmah(id INTEGER PRIMARY KEY "
    "AUTOINCREMENT, name TEXT, page INTEGER, surah INTEGER, number INTEGER)");
  QString q;
  if (id == -1) {
    q = "INSERT INTO khatmah(name, page, surah, number) VALUES ('%0', %1, %2, "
        "%3)";
    dbQuery.prepare(q.arg(name,
                          QString::number(v.page),
                          QString::number(v.surah),
                          QString::number(v.number)));
  } else {
    q = "REPLACE INTO khatmah VALUES "
        "(%0, "
        "'%1', %2, %3, %4)";
    dbQuery.prepare(q.arg(QString::number(id),
                          name,
                          QString::number(v.page),
                          QString::number(v.surah),
                          QString::number(v.number)));
  }

  if (!dbQuery.exec()) {
    qCritical() << "Couldn't create new khatmah entry!";
    qDebug() << m_openDBCon.lastError();
    return -1;
  }

  if (id != -1)
    return id;

  dbQuery.exec("SELECT id FROM khatmah ORDER BY id DESC limit 1");
  dbQuery.next();
  return dbQuery.value(0).toInt();
}

bool
DBManager::editKhatmahName(const int khatmahId, QString newName)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  QString q = "SELECT DISTINCT id FROM khatmah WHERE name='%0'";
  if (!dbQuery.exec(q.arg(newName))) {
    qCritical() << "Couldn't execute sql query: " << dbQuery.lastQuery();
    qDebug() << m_openDBCon.lastError();
    return false;
  }
  if (dbQuery.next())
    return false;

  q = "UPDATE khatmah SET name='%0' WHERE id=%1";
  if (!dbQuery.exec(q.arg(newName, QString::number(khatmahId)))) {
    qCritical() << "Couldn't rename khatmah entry!";
    qDebug() << m_openDBCon.lastError();
    return false;
  }

  m_openDBCon.commit();
  return true;
}

void
DBManager::removeKhatmah(const int id)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  if (!dbQuery.exec(QString::asprintf("DELETE FROM khatmah WHERE id=%i", id)))
    qDebug() << "Couldn't execute query: " << dbQuery.lastQuery();
}

bool
DBManager::saveActiveKhatmah(const Verse& v)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  QString q = QString::asprintf(
    "UPDATE khatmah SET page=%i, surah=%i, number=%i WHERE id=%i",
    v.page,
    v.surah,
    v.number,
    m_activeKhatmah);
  if (!dbQuery.exec(q)) {
    qCritical() << "Couldn't save position in mushaf";
    return false;
  }
  if (!m_openDBCon.commit())
    return false;

  return true;
}

QString
DBManager::getVerseText(const int sIdx, const int vIdx)
{
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);
  if (m_verseType == VerseType::annotated)
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

QList<int>
DBManager::getAllKhatmah()
{
  QList<int> res;
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  if (!dbQuery.exec("SELECT id FROM khatmah"))
    qCritical() << "Couldn't execute sql query: " << dbQuery.lastQuery();

  while (dbQuery.next())
    res.append(dbQuery.value(0).toInt());

  return res;
}

QString
DBManager::getKhatmahName(const int id)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  if (!dbQuery.exec("SELECT name FROM khatmah WHERE id=" + QString::number(id)))
    qCritical() << "Couldn't execute sql query: " << dbQuery.lastQuery();

  dbQuery.next();
  return dbQuery.value(0).toString();
}

QPair<Verse, QString>
DBManager::randomVerse()
{
  QPair<Verse, QString> res;
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

  int id = QRandomGenerator::global()->bounded(1, 6237);
  dbQuery.prepare(
    "SELECT page,sura_no,aya_no,aya_text FROM verses_v1 WHERE id=" +
    QString::number(id));

  if (!dbQuery.exec()) {
    qCritical() << "Error occurred during randomVerse SQL statment exec";
  }
  dbQuery.next();
  Verse v{ dbQuery.value(0).toInt(),
           dbQuery.value(1).toInt(),
           dbQuery.value(2).toInt() };

  res.first = v;
  res.second = dbQuery.value(3).toString();
  return res;
}

int
DBManager::getVersePage(const int& surahIdx, const int& verse)
{
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

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

QList<Verse>
DBManager::searchVerses(QString searchText,
                        const int range[2],
                        const bool whole)
{
  QList<Verse> results;
  setOpenDatabase(Database::quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

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
    Verse entry{ dbQuery.value(0).toInt(),
                 dbQuery.value(1).toInt(),
                 dbQuery.value(2).toInt() };
    results.append(entry);
  }

  return results;
}

QList<Verse>
DBManager::bookmarkedVerses(int surahIdx)
{
  QList<Verse> results;
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  QString q = "SELECT page,surah,number FROM favorites";
  if (surahIdx != -1)
    q.append(" WHERE surah=" + QString::number(surahIdx));

  dbQuery.prepare(q.append(" ORDER BY surah, number"));
  if (!dbQuery.exec())
    qCritical() << "Couldn't execute bookmarkedVerses SELECT query";

  while (dbQuery.next()) {
    results.append(Verse{ dbQuery.value(0).toInt(),
                          dbQuery.value(1).toInt(),
                          dbQuery.value(2).toInt() });
  }

  return results;
}

bool
DBManager::isBookmarked(Verse v)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare(
    "SELECT page FROM favorites WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, v.page);
  dbQuery.bindValue(1, v.surah);
  dbQuery.bindValue(2, v.number);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't check if verse is bookmarked";
    return false;
  }

  dbQuery.next();

  return dbQuery.isValid();
}

bool
DBManager::addBookmark(Verse v)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.exec("CREATE TABLE IF NOT EXISTS favorites(id INTEGER PRIMARY KEY "
               "AUTOINCREMENT,"
               "page INTEGER, surah INTEGER, number INTEGER)");

  dbQuery.prepare(
    "INSERT INTO favorites(page, surah, number) VALUES (:p, :s, :n)");
  dbQuery.bindValue(0, v.page);
  dbQuery.bindValue(1, v.surah);
  dbQuery.bindValue(2, v.number);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't add verse to bookmarks db";
    return false;
  }

  if (!m_openDBCon.commit())
    return false;

  return true;
}

bool
DBManager::removeBookmark(Verse v)
{
  setOpenDatabase(Database::bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.prepare(
    "DELETE FROM favorites WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, v.page);
  dbQuery.bindValue(1, v.surah);
  dbQuery.bindValue(2, v.number);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't remove verse from bookmarks";
    return false;
  }

  return true;
}

/* ---------------- Side content methods ---------------- */

QString
DBManager::getTafsir(const int sIdx, const int vIdx)
{
  setOpenDatabase(Database::tafsir, m_tafsirDbPath.filePath());

  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare("SELECT text FROM content WHERE sura=:s AND aya=:v");
  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec()) {
    qFatal("Couldn't execute getTafsir query!");
  }

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QString
DBManager::getTranslation(const int sIdx, const int vIdx)
{
  setOpenDatabase(Database::translation, m_transDbPath.filePath());

  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare("SELECT text FROM content WHERE sura=:s AND aya=:v");
  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec())
    qFatal("Couldn't execute getTranslation query!");

  dbQuery.next();

  return dbQuery.value(0).toString();
}

void
DBManager::setActiveKhatmah(const int id)
{
  m_activeKhatmah = id;
}

void
DBManager::setVerseType(VerseType newVerseType)
{
    m_verseType = newVerseType;
}

VerseType
DBManager::getVerseType() const
{
    return m_verseType;
}

const int
DBManager::activeKhatmah() const
{
  return m_activeKhatmah;
}

const Tafsir*
DBManager::currTafsir() const
{
  return m_currTafsir;
}
