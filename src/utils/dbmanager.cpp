/**
 * @file dbmanager.cpp
 * @brief Implementation for DBManager
 */

#include "dbmanager.h"

QSharedPointer<DBManager>
DBManager::current()
{
  static QSharedPointer<DBManager> controller =
    QSharedPointer<DBManager>::create();
  return controller;
}

DBManager::DBManager(QObject* parent)
  : QObject(parent)
{
  m_quranDbPath.setFile(m_assetsDir->filePath("quran.db"));
  m_glyphsDbPath.setFile(m_assetsDir->filePath("glyphs.db"));
  m_betaqatDbPath.setFile(m_assetsDir->filePath("betaqat.db"));

  // set database driver, set the path & open a connection with the db
  QSqlDatabase::addDatabase("QSQLITE", "QuranCon");
  QSqlDatabase::addDatabase("QSQLITE", "GlyphsCon");
  QSqlDatabase::addDatabase("QSQLITE", "BetaqatCon");
  QSqlDatabase::addDatabase("QSQLITE", "BookmarksCon");
  QSqlDatabase::addDatabase("QSQLITE", "TafsirCon");
  QSqlDatabase::addDatabase("QSQLITE", "TranslationCon");

  for (int i = 1; i <= 114; i++)
    m_surahNames.append(getSurahName(i));

  updateLoadedTafsir();
  updateLoadedTranslation();
}

/* ---------------- Database handling ---------------- */

void
DBManager::setOpenDatabase(Database db, QString path)
{
  if (m_currentDb == db)
    return;

  m_currentDb = db;
  m_openDBCon.close();
  switch (db) {
    case Null:
      break;

    case Quran:
      m_openDBCon = QSqlDatabase::database("QuranCon");
      break;

    case Glyphs:
      m_openDBCon = QSqlDatabase::database("GlyphsCon");
      break;

    case Bookmarks:
      m_openDBCon = QSqlDatabase::database("BookmarksCon");
      break;

    case Tafsir:
      m_openDBCon = QSqlDatabase::database("TafsirCon");
      break;

    case Translation:
      m_openDBCon = QSqlDatabase::database("TranslationCon");
      break;

    case Betaqat:
      m_openDBCon = QSqlDatabase::database("BetaqatCon");
      break;
  }

  updateOpenDbFile(path);
}

void
DBManager::updateOpenDbFile(const QString& filepath)
{
  m_openDBCon.setDatabaseName(filepath);
  if (!m_openDBCon.open())
    qFatal("Couldn't open Database!");
}

bool
DBManager::setCurrentTafsir(int idx)
{
  if (idx < 0 || idx >= m_tafasir.size())
    return false;
  if (m_currTafsir == m_tafasir[idx])
    return true;

  m_currTafsir = m_tafasir[idx];
  const QDir& baseDir =
    m_currTafsir->isExtra() ? *m_downloadsDir : *m_assetsDir;
  QString path = "tafasir/" + m_currTafsir->filename();
  if (!baseDir.exists(path))
    return false;

  m_tafsirDbPath.setFile(baseDir.filePath(path));
  updateOpenDbFile(m_tafsirDbPath.absoluteFilePath());
  return true;
}

bool
DBManager::setCurrentTranslation(int idx)
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

  m_transDbPath.setFile(baseDir.filePath(path));
  updateOpenDbFile(m_transDbPath.absoluteFilePath());
  return true;
}

/* ---------------- Page-related methods ---------------- */

QPair<int, int>
DBManager::getPageMetadata(const int page)
{
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.prepare(
    "SELECT sura_no,jozz FROM verses_v1 WHERE page=:p ORDER BY id");
  dbQuery.bindValue(0, page);

  if (!dbQuery.exec())
    qCritical() << "Error occurred during getPageMetadata SQL statment exec";

  dbQuery.next();
  // { surahIdx, jozz }
  return { dbQuery.value(0).toInt(), dbQuery.value(1).toInt() };
}

QStringList
DBManager::getPageLines(const int page)
{
  setOpenDatabase(Database::Glyphs, m_glyphsDbPath.filePath());
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

QList<QList<int>>
DBManager::getVerseInfoList(int page)
{
  QList<QList<int>> viList;
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

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

int
DBManager::getJuzStartPage(const int juz)
{
  setOpenDatabase(Database::Glyphs, m_glyphsDbPath.filePath());
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
  setOpenDatabase(Database::Glyphs, m_glyphsDbPath.filePath());
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
  setOpenDatabase(Database::Glyphs, m_glyphsDbPath.filePath());
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
  setOpenDatabase(Database::Glyphs, m_glyphsDbPath.filePath());
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
    if (m_verseType != VerseType::Qcf)
    return getVerseText(sIdx, vIdx);

  setOpenDatabase(Database::Glyphs, m_glyphsDbPath.filePath());

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
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
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
  setOpenDatabase(Betaqat, m_betaqatDbPath.filePath());
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
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
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

QList<int>
DBManager::getVerseById(const int id)
{
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);
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
DBManager::getSurahVerseCount(const int surahIdx)
{
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
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
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
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

QList<QList<int>>
DBManager::searchSurahs(QString searchText,
                        const QList<int> surahs,
                        const bool whole)
{
  QList<QList<int>> results;
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
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
    results.append({ dbQuery.value(0).toInt(),
                     dbQuery.value(1).toInt(),
                     dbQuery.value(2).toInt() });
  }

  return results;
}

QList<int>
DBManager::searchSurahNames(QString text)
{
  QList<int> results;
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
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
DBManager::loadVerse(const int khatmahId, QList<int>& vInfo)
{
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);

  QString q = QString::asprintf(
    "SELECT page,surah,number FROM khatmah WHERE id=%i", khatmahId);
  if (!dbQuery.exec(q)) {
    qCritical() << "Couldn't execute getPosition SQL query!";
    return false;
  }
  if (!dbQuery.next())
    return false;

  vInfo[0] = dbQuery.value(0).toInt();
  vInfo[1] = dbQuery.value(1).toInt();
  vInfo[2] = dbQuery.value(2).toInt();
  return true;
}

int
DBManager::addKhatmah(QList<int> vInfo, const QString name, const int id)
{
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.exec(
    "CREATE TABLE IF NOT EXISTS khatmah(id INTEGER PRIMARY KEY "
    "AUTOINCREMENT, name TEXT, page INTEGER, surah INTEGER, number INTEGER)");
  QString q;
  if (id == -1) {
    q = "INSERT INTO khatmah(name, page, surah, number) VALUES ('%0', %1, %2, "
        "%3)";
    dbQuery.prepare(q.arg(name,
                          QString::number(vInfo[0]),
                          QString::number(vInfo[1]),
                          QString::number(vInfo[2])));
  } else {
    q = "REPLACE INTO khatmah VALUES "
        "(%0, "
        "'%1', %2, %3, %4)";
    dbQuery.prepare(q.arg(QString::number(id),
                          name,
                          QString::number(vInfo[0]),
                          QString::number(vInfo[1]),
                          QString::number(vInfo[2])));
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
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
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
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  if (!dbQuery.exec(QString::asprintf("DELETE FROM khatmah WHERE id=%i", id)))
    qDebug() << "Couldn't execute query: " << dbQuery.lastQuery();
}

bool
DBManager::saveActiveKhatmah(QList<int> vInfo)
{
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  QString q = QString::asprintf(
    "UPDATE khatmah SET page=%i, surah=%i, number=%i WHERE id=%i",
    vInfo[0],
    vInfo[1],
    vInfo[2],
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
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);
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

QList<int>
DBManager::getAllKhatmah()
{
  QList<int> res;
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
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
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  if (!dbQuery.exec("SELECT name FROM khatmah WHERE id=" + QString::number(id)))
    qCritical() << "Couldn't execute sql query: " << dbQuery.lastQuery();

  dbQuery.next();
  return dbQuery.value(0).toString();
}

QList<int>
DBManager::randomVerse()
{
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
  QSqlQuery dbQuery(m_openDBCon);

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

int
DBManager::getVersePage(const int& surahIdx, const int& verse)
{
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
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

QList<QList<int>>
DBManager::searchVerses(QString searchText,
                        const int range[2],
                        const bool whole)
{
  QList<QList<int>> results;
  setOpenDatabase(Database::Quran, m_quranDbPath.filePath());
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
    QList<int> entry{ dbQuery.value(0).toInt(),
                      dbQuery.value(1).toInt(),
                      dbQuery.value(2).toInt() };
    results.append(entry);
  }

  return results;
}

QList<QList<int>>
DBManager::bookmarkedVerses(int surahIdx)
{
  QList<QList<int>> results;
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  QString q = "SELECT page,surah,number FROM favorites";
  if (surahIdx != -1)
    q.append(" WHERE surah=" + QString::number(surahIdx));

  dbQuery.prepare(q.append(" ORDER BY surah, number"));
  if (!dbQuery.exec())
    qCritical() << "Couldn't execute bookmarkedVerses SELECT query";

  while (dbQuery.next()) {
    results.append({ dbQuery.value(0).toInt(),
                     dbQuery.value(1).toInt(),
                     dbQuery.value(2).toInt() });
  }

  return results;
}

bool
DBManager::isBookmarked(QList<int> vInfo)
{
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare(
    "SELECT page FROM favorites WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, vInfo[0]);
  dbQuery.bindValue(1, vInfo[1]);
  dbQuery.bindValue(2, vInfo[2]);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't check if verse is bookmarked";
    return false;
  }

  dbQuery.next();

  return dbQuery.isValid();
}

bool
DBManager::addBookmark(QList<int> vInfo)
{
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.exec("CREATE TABLE IF NOT EXISTS favorites(id INTEGER PRIMARY KEY "
               "AUTOINCREMENT,"
               "page INTEGER, surah INTEGER, number INTEGER)");

  dbQuery.prepare(
    "INSERT INTO favorites(page, surah, number) VALUES (:p, :s, :n)");
  dbQuery.bindValue(0, vInfo[0]);
  dbQuery.bindValue(1, vInfo[1]);
  dbQuery.bindValue(2, vInfo[2]);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't add verse to bookmarks db";
    return false;
  }

  m_openDBCon.commit();
  emit bookmarkAdded();
  return true;
}

bool
DBManager::removeBookmark(QList<int> vInfo)
{
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.prepare(
    "DELETE FROM favorites WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, vInfo[0]);
  dbQuery.bindValue(1, vInfo[1]);
  dbQuery.bindValue(2, vInfo[2]);

  if (!dbQuery.exec()) {
    qWarning() << "Couldn't remove verse from bookmarks";
    return false;
  }

  emit bookmarkRemoved();
  return true;
}

/* ---------------- Side content methods ---------------- */

QString
DBManager::getTafsir(const int sIdx, const int vIdx)
{
  setOpenDatabase(Database::Tafsir, m_tafsirDbPath.filePath());

  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare("SELECT text FROM content WHERE sura=:s AND aya=:v");
  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec())
    qCritical("Couldn't execute getTafsir query!");

  dbQuery.next();

  return dbQuery.value(0).toString();
}

QString
DBManager::getTranslation(const int sIdx, const int vIdx)
{
  setOpenDatabase(Database::Translation, m_transDbPath.filePath());

  QSqlQuery dbQuery(m_openDBCon);

  dbQuery.prepare("SELECT text FROM content WHERE sura=:s AND aya=:v");
  dbQuery.bindValue(0, sIdx);
  dbQuery.bindValue(1, vIdx);

  if (!dbQuery.exec())
    qCritical("Couldn't execute getTranslation query!");

  dbQuery.next();

  return dbQuery.value(0).toString();
}

void
DBManager::saveThoughts(QList<int> vInfo, const QString& text)
{
  int id = getVerseId(vInfo[1], vInfo[2]);
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.exec("CREATE TABLE IF NOT EXISTS thoughts(id INTEGER PRIMARY KEY "
               "UNIQUE,"
               "page INTEGER, surah INTEGER, number INTEGER, text TEXT)");

  dbQuery.prepare("REPLACE INTO thoughts(id, page, surah, number, text) "
                  "VALUES(:i, :p, :s, :n, :t)");
  dbQuery.bindValue(0, id);
  dbQuery.bindValue(1, vInfo[0]);
  dbQuery.bindValue(2, vInfo[1]);
  dbQuery.bindValue(3, vInfo[2]);
  dbQuery.bindValue(4, text);

  if (!dbQuery.exec())
    qCritical() << "SQL statement execution error:" << dbQuery.lastError();

  m_openDBCon.commit();
}

QString
DBManager::getThoughts(QList<int> vInfo)
{
  setOpenDatabase(Database::Bookmarks, m_bookmarksFilepath);
  QSqlQuery dbQuery(m_openDBCon);
  dbQuery.prepare(
    "SELECT text FROM thoughts WHERE page=:p AND surah=:s AND number=:n");
  dbQuery.bindValue(0, vInfo[0]);
  dbQuery.bindValue(1, vInfo[1]);
  dbQuery.bindValue(2, vInfo[2]);

  if (!dbQuery.exec())
    qCritical() << "SQL statement execution error:" << dbQuery.lastError();

  dbQuery.next();
  return dbQuery.value(0).toString();
}

void
DBManager::updateLoadedTafsir()
{
  int currTafsir = m_settings->value("Reader/Tafsir").toInt();
  setCurrentTafsir(currTafsir);
}

void
DBManager::updateLoadedTranslation()
{
  int currTrans = m_settings->value("Reader/Translation").toInt();
  setCurrentTranslation(currTrans);
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

QSharedPointer<Tafsir>
DBManager::currTafsir() const
{
  return m_currTafsir;
}

QSharedPointer<Translation>
DBManager::currTranslation() const
{
  return m_currTr;
}
