#include "dbmanager.h"

DBManager::DBManager(QObject *parent, int qcfVersion)
    : QObject(parent)
    , m_qcfVer{qcfVersion}
{
    m_quranDbPath.setFile(m_dbDir.filePath("quran.db"));
    m_glyphsDbPath.setFile(m_dbDir.filePath("glyphs.db"));
    m_bookmarksDbPath.setFile(QDir::currentPath() + QDir::separator() + "bookmarks.db");

    // set database driver, set the path & open a connection with the db
    QSqlDatabase::addDatabase("QSQLITE", "QuranCon");
    QSqlDatabase::addDatabase("QSQLITE", "GlyphsCon");
    QSqlDatabase::addDatabase("QSQLITE", "BookmarksCon");
    QSqlDatabase::addDatabase("QSQLITE", "TafsirCon");
    QSqlDatabase::addDatabase("QSQLITE", "TranslationCon");
}

/* ---------------- Database handling ---------------- */

void DBManager::setOpenDatabase(Database db, QString filePath)
{
    m_openDBCon.close();

    switch (db) {
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
    }

    m_openDBCon.setDatabaseName(filePath);
    if (!m_openDBCon.open())
        qFatal("Couldn't open Database!");
}

void DBManager::setCurrentTafsir(Tafsir tafsirName)
{
    m_dbDir.cd("tafasir");
    m_currTafsir = tafsirName;

    switch (m_currTafsir) {
    case Tafsir::muyassar:
        m_tafsirDbFilename = "muyassar.db";
        break;

    case Tafsir::baghawy:
        m_tafsirDbFilename = "baghawy.db";
        break;

    case Tafsir::e3rab:
        m_tafsirDbFilename = "e3rab.db";
        break;

    case Tafsir::indonesian:
        m_tafsirDbFilename = "indonesian.db";
        break;

    case Tafsir::katheer:
        m_tafsirDbFilename = "katheer.db";
        break;

    case Tafsir::qortoby:
        m_tafsirDbFilename = "qortoby.db";
        break;

    case Tafsir::russian:
        m_tafsirDbFilename = "russian.db";
        break;

    case Tafsir::sa3dy:
        m_tafsirDbFilename = "sa3dy.db";
        break;

    case Tafsir::tabary:
        m_tafsirDbFilename = "tabary.db";
        break;

    case Tafsir::tafheem:
        m_tafsirDbFilename = "tafheem.db";
        break;

    case Tafsir::tanweer:
        m_tafsirDbFilename = "tanweer.db";
        break;

    case Tafsir::waseet:
        m_tafsirDbFilename = "waseet.db";
        break;

    default:
        m_tafsirDbFilename = "muyassar.db";
        break;
    }

    m_tafsirDbPath.setFile(m_dbDir.filePath(m_tafsirDbFilename));
    m_dbDir.cdUp();
}

void DBManager::setCurrentTranslation(Translation translationName)
{
    m_dbDir.cd("translations");
    m_currTrans = translationName;

    switch (m_currTrans) {
    case ar_ma3any:
        m_transDbFilename = "ar_ma3any.db";
        break;
    case bn_bengali:
        m_transDbFilename = "bn_bengali.db";
        break;
    case bs_korkut:
        m_transDbFilename = "bs_korkut.db";
        break;
    case de_bubenheim:
        m_transDbFilename = "de_bubenheim.db";
        break;
    case en_sahih:
        m_transDbFilename = "en_sahih.db";
        break;
    case es_navio:
        m_transDbFilename = "es_navio.db";
        break;
    case fr_hamidullah:
        m_transDbFilename = "fr_hamidullah.db";
        break;
    case ha_gumi:
        m_transDbFilename = "ha_gumi.db";
        break;
    case id_indonesian:
        m_transDbFilename = "id_indonesian.db";
        break;
    case it_piccardo:
        m_transDbFilename = "it_piccardo.db";
        break;
    case ku_asan:
        m_transDbFilename = "ku_asan.db";
        break;
    case ml_abdulhameed:
        m_transDbFilename = "ml_abdulhameed.db";
        break;
    case ms_basmeih:
        m_transDbFilename = "ms_basmeih.db";
        break;
    case nl_siregar:
        m_transDbFilename = "nl_siregar.db";
        break;
    case pr_tagi:
        m_transDbFilename = "pr_tagi.db";
        break;
    case pt_elhayek:
        m_transDbFilename = "pt_elhayek.db";
        break;
    case ru_kuliev:
        m_transDbFilename = "ru_kuliev.db";
        break;
    case so_abduh:
        m_transDbFilename = "so_abduh.db";
        break;
    case sq_nahi:
        m_transDbFilename = "sq_nahi.db";
        break;
    case sv_bernstrom:
        m_transDbFilename = "sv_bernstrom.db";
        break;
    case sw_barwani:
        m_transDbFilename = "sw_barwani.db";
        break;
    case ta_tamil:
        m_transDbFilename = "ta_tamil.db";
        break;
    case th_thai:
        m_transDbFilename = "th_thai.db";
        break;
    case tr_diyanet:
        m_transDbFilename = "tr_diyanet.db";
        break;
    case ur_jalandhry:
        m_transDbFilename = "ur_jalandhry.db";
        break;
    case uz_sodik:
        m_transDbFilename = "uz_sodik.db";
        break;
    case zh_jian:
        m_transDbFilename = "zh_jian.db";
        break;
    default:
        m_transDbFilename = "en_sahih.db";
        break;
    }

    m_transDbPath.setFile(m_dbDir.filePath(m_transDbFilename));
    m_dbDir.cdUp();
}

/* ---------------- Page-related methods ---------------- */

QList<int> DBManager::getVerseBounds(const int surah, const int ayah)
{
    QList<int> bounds;
    setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    QString q = "SELECT start_pos,end_pos FROM %0 WHERE surah=%1 AND ayah=%2";
    dbQuery.prepare(q.arg("coordinates_v" + QString::number(m_qcfVer),
                          QString::number(surah),
                          QString::number(ayah)));

    if (!dbQuery.exec())
        qWarning() << "Couldn't execute DBManager::getVerseBounds query!";

    dbQuery.next();
    bounds.append(dbQuery.value(0).toInt());
    bounds.append(dbQuery.value(1).toInt());

    return bounds;
}

QList<int> DBManager::getPageMetadata(const int page)
{
    QList<int> data; // { surahIdx, jozz }
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);
    dbQuery.prepare("SELECT sura_no,jozz FROM verses_v1 WHERE page=:p ORDER BY id");
    dbQuery.bindValue(0, page);

    if (!dbQuery.exec())
        qCritical() << "Error occurred during getFirstSurahInPage SQL statment exec";

    dbQuery.next();
    data.push_back(dbQuery.value(0).toInt());
    data.push_back(dbQuery.value(1).toInt());

    return data;
}

QStringList DBManager::getPageLines(const int page)
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

QList<DBManager::Verse> DBManager::getVerseInfoList(int page)
{
    QList<Verse> viList;
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    QString query = "SELECT sura_no,aya_no FROM verses_v%0 WHERE page=%1 ORDER BY id";
    dbQuery.prepare(query.arg(QString::number(m_qcfVer), QString::number(page)));

    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }

    while (dbQuery.next()) {
        Verse v{page, dbQuery.value(0).toInt(), dbQuery.value(1).toInt()};
        viList.append(v);
    }

    return viList;
}

/* ---------------- Glyph-related methods ---------------- */

QString DBManager::getSurahNameGlyph(const int sura)
{
    setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    dbQuery.prepare("SELECT qcf_v1 FROM surah_glyphs WHERE surah=:i");
    dbQuery.bindValue(0, sura);
    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }

    dbQuery.next();

    return dbQuery.value(0).toString();
}

QString DBManager::getJuzGlyph(const int juz)
{
    setOpenDatabase(Database::glyphs, m_glyphsDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    dbQuery.prepare("SELECT qcf_v1 FROM juzz_glyphs WHERE juzz=:j");
    dbQuery.bindValue(0, juz);
    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during getJuzGlyph SQL statment exec";
    }

    dbQuery.next();

    return dbQuery.value(0).toString();
}

QString DBManager::getVerseGlyphs(const int sIdx, const int vIdx)
{
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

QString DBManager::getSurahName(const int sIdx, bool en)
{
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    if (en) {
        dbQuery.prepare("SELECT sura_name_en FROM verses_v1 WHERE sura_no=:i");
    } else {
        dbQuery.prepare("SELECT sura_name_ar FROM verses_v1 WHERE sura_no=:i");
    }

    dbQuery.bindValue(0, sIdx);
    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }

    dbQuery.next();
    return dbQuery.value(0).toString();
}

int DBManager::getSurahVerseCount(const int surahIdx)
{
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    dbQuery.prepare("SELECT aya_no FROM verses_v1 WHERE sura_no=:idx ORDER BY aya_no DESC");
    dbQuery.bindValue(0, surahIdx);

    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during SQL statment exec";
        return -1;
    }

    dbQuery.next();
    return dbQuery.value(0).toInt();
}

int DBManager::getSurahStartPage(int surahIdx)
{
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    dbQuery.prepare("SELECT page FROM verses_v1 WHERE sura_no=:sn AND aya_no=1");
    dbQuery.bindValue(0, surahIdx);

    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }
    dbQuery.next();

    return dbQuery.value(0).toInt();
}

QList<int> DBManager::searchSurahs(QString searchText)
{
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QList<int> results;
    QSqlQuery dbQuery(m_openDBCon);

    dbQuery.prepare(
        "SELECT sura_no FROM verses_v1 WHERE sura_name_ar like '%:searchText%' ORDER BY id");
    dbQuery.bindValue(0, searchText);
    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during searchVerses 1st SQL statment exec";
    }

    while (dbQuery.next()) {
        results.append(dbQuery.value(0).toInt());
    }

    return results;
}

/* ---------------- Verse-related methods ---------------- */

QString DBManager::getVerseText(const int sIdx, const int vIdx)
{
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    dbQuery.prepare("SELECT aya_text FROM verses_v1 WHERE sura_no=:s AND aya_no=:v");
    dbQuery.bindValue(0, sIdx);
    dbQuery.bindValue(1, vIdx);

    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }
    dbQuery.next();

    return dbQuery.value(0).toString();
}

int DBManager::getVersePage(const int &surahIdx, const int &verse)
{
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    QString query = "SELECT page FROM verses_v%0 WHERE sura_no=%1 AND aya_no=%2";
    dbQuery.prepare(
        query.arg(QString::number(m_qcfVer), QString::number(surahIdx), QString::number(verse)));

    if (!dbQuery.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }
    dbQuery.next();

    return dbQuery.value(0).toInt();
}

QList<DBManager::Verse> DBManager::searchVerses(QString searchText, int range[2])
{
    QList<DBManager::Verse> results;
    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    QString q = "SELECT page,sura_no,aya_no FROM verses_v" + QString::number(m_qcfVer)
                + " WHERE page >= " + QString::number(range[0])
                + " AND page <= " + QString::number(range[1]) + " AND aya_text_emlaey like'%"
                + searchText + "%' ORDER BY id";

    dbQuery.prepare(q);
    if (!dbQuery.exec()) {
        qCritical() << "[CRITICAL] Error occurred during searchVerses SQL statment exec";
    }

    while (dbQuery.next()) {
        Verse entry{dbQuery.value(0).toInt(), dbQuery.value(1).toInt(), dbQuery.value(2).toInt()};
        results.append(entry);
    }

    return results;
}

QList<DBManager::Verse> DBManager::favoriteVerses()
{
    QList<Verse> results;
    setOpenDatabase(Database::bookmarks, m_bookmarksDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    dbQuery.prepare("SELECT page,surah,number FROM favorites ORDER BY id");
    if (!dbQuery.exec())
        qCritical() << "Couldn't execute favoriteVerses SELECT query";

    while (dbQuery.next()) {
        results.append(
            Verse{dbQuery.value(0).toInt(), dbQuery.value(1).toInt(), dbQuery.value(2).toInt()});
    }

    return results;
}

bool DBManager::isBookmarked(Verse v)
{
    setOpenDatabase(Database::bookmarks, m_bookmarksDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);

    dbQuery.prepare("SELECT page FROM favorites WHERE page=:p AND surah=:s AND number=:n");
    dbQuery.bindValue(0, v.page);
    dbQuery.bindValue(1, v.surah);
    dbQuery.bindValue(2, v.number);

    if (!dbQuery.exec()) {
        qWarning() << "Couldn't check if verse is favorites";
        return false;
    }

    dbQuery.next();

    return dbQuery.isValid();
}

bool DBManager::addBookmark(Verse v)
{
    setOpenDatabase(Database::bookmarks, m_bookmarksDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);
    dbQuery.exec("CREATE TABLE IF NOT EXISTS favorites(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                 "page INTEGER, surah INTEGER, number INTEGER)");
    // code to check whether verse exists in db or no

    dbQuery.clear();
    dbQuery.prepare("INSERT INTO favorites(page, surah, number) VALUES (:p, :s, :n)");
    dbQuery.bindValue(0, v.page);
    dbQuery.bindValue(1, v.surah);
    dbQuery.bindValue(2, v.number);

    if (!dbQuery.exec()) {
        qWarning() << "Couldn't add verse to favorites db";
        return false;
    }

    if (!m_openDBCon.commit())
        return false;

    return true;
}

bool DBManager::removeBookmark(Verse v)
{
    setOpenDatabase(Database::bookmarks, m_bookmarksDbPath.filePath());
    QSqlQuery dbQuery(m_openDBCon);
    // CODE TO REMOVE ENTRY FROM SQL TABLE
    dbQuery.prepare("DELETE FROM favorites WHERE page=:p AND surah=:s AND number=:n");
    dbQuery.bindValue(0, v.page);
    dbQuery.bindValue(1, v.surah);
    dbQuery.bindValue(2, v.number);

    if (!dbQuery.exec()) {
        qWarning() << "Couldn't remove verse from favorites";
        return false;
    }

    return true;
}

/* ---------------- Side content methods ---------------- */

QString DBManager::getTafsir(const int sIdx, const int vIdx)
{
    setOpenDatabase(Database::tafsir, m_tafsirDbPath.filePath());

    QSqlQuery dbQuery(m_openDBCon);

    QString q = "SELECT text FROM %0 WHERE sura=%1 AND aya=%2";
    q = q.arg(m_tafsirDbPath.baseName()).arg(sIdx).arg(vIdx);
    dbQuery.prepare(q);

    if (!dbQuery.exec()) {
        qFatal("Couldn't execute getTafsir query!");
    }

    dbQuery.next();

    return dbQuery.value(0).toString();
}

QString DBManager::getTranslation(const int sIdx, const int vIdx)
{
    setOpenDatabase(Database::translation, m_transDbPath.filePath());

    QSqlQuery dbQuery(m_openDBCon);

    QString q = "SELECT text FROM %0 WHERE sura=%1 AND aya=%2";
    q = q.arg(m_transDbPath.baseName()).arg(sIdx).arg(vIdx);
    dbQuery.prepare(q);

    if (!dbQuery.exec())
        qFatal("Couldn't execute getTrans query!");

    dbQuery.next();

    return dbQuery.value(0).toString();
}
