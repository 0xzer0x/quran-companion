#include "../headers/dbmanager.h"

DBManager::DBManager(QObject *parent)
    : QObject{parent}
{
    m_quranDbPath.setFile(m_dbDir.filePath("quran.db"));

    // set database driver, set the path & open a connection with the db
    QSqlDatabase::addDatabase("QSQLITE", "QuranData");
    QSqlDatabase::addDatabase("QSQLITE", "TafsirData");
    QSqlDatabase::addDatabase("QSQLITE", "TranslationData");

    setOpenDatabase(Database::quran, m_quranDbPath.filePath());
}

/* ---------------- Database handling ---------------- */

void DBManager::setOpenDatabase(Database db, QString filePath)
{
    m_openDBCon.close();

    switch (db) {
    case quran:
        m_openDBCon = QSqlDatabase::database("QuranData");
        break;

    case tafsir:
        m_openDBCon = QSqlDatabase::database("TafsirData");
        break;

    case translation:
        m_openDBCon = QSqlDatabase::database("TranslationData");
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
    qInfo() << m_tafsirDbPath.filePath();
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
    qInfo() << m_transDbPath.filePath();
    m_dbDir.cdUp();
}

/* ---------------- Main Quran page data ---------------- */

QList<int> DBManager::getVerseBounds(const int surah, const int ayah)
{
    QList<int> bounds;

    m_queryOpenDB = QSqlQuery(m_openDBCon);
    QString q = "SELECT start_pos,end_pos FROM coordinates WHERE surah=%0 AND ayah=%1";
    m_queryOpenDB.prepare(q.arg(surah).arg(ayah));

    if (!m_queryOpenDB.exec())
        qWarning() << "Couldn't execute DBManager::getVerseBounds query!";

    m_queryOpenDB.next();
    bounds.append(m_queryOpenDB.value(0).toInt());
    bounds.append(m_queryOpenDB.value(1).toInt());

    return bounds;
}

int DBManager::getSurahIdx(QString sName)
{
    // create the sql query string
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT sura_no FROM verses WHERE sura_name_ar=:name AND aya_no=1");
    m_queryOpenDB.bindValue(":name", sName);

    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during SQL statment exec";
        return -1;
    }

    // return the integer idx of the first record in the result
    m_queryOpenDB.next();
    return m_queryOpenDB.value(0).toInt();
}

int DBManager::getSurahVerseCount(const int surahIdx)
{
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT aya_no FROM verses WHERE sura_no=:idx ORDER BY aya_no DESC");
    m_queryOpenDB.bindValue(0, surahIdx);

    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during SQL statment exec";
        return -1;
    }

    m_queryOpenDB.next();
    return m_queryOpenDB.value(0).toInt();
}

QList<int> DBManager::getPageMetadata(const int page)
{
    QList<int> data; // { surahIdx, jozz }
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT sura_no,jozz FROM verses WHERE page=:p ORDER BY id");
    m_queryOpenDB.bindValue(0, page);

    if (!m_queryOpenDB.exec())
        qCritical() << "Error occurred during getFirstSurahInPage SQL statment exec";

    m_queryOpenDB.next();
    data.push_back(m_queryOpenDB.value(0).toInt());
    data.push_back(m_queryOpenDB.value(1).toInt());

    return data;
}

QList<QString> DBManager::getVersesInPage(const int page)
{
    QList<QString> verseList;
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT aya_text FROM verses WHERE page=:p ORDER BY id");
    m_queryOpenDB.bindValue(0, page);

    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during getVersesInPage SQL statment exec";

    } else {
        while (m_queryOpenDB.next()) {
            verseList.append(m_queryOpenDB.value(0).toString());
        }
    }

    return verseList;
}

QList<QMap<QString, int>> DBManager::getVerseInfoList(int page)
{
    QList<QMap<QString, int>> viList;
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT sura_no,aya_no,sura_name_ar FROM verses WHERE page=:p");
    m_queryOpenDB.bindValue(0, page);

    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }

    while (m_queryOpenDB.next()) {
        QMap<QString, int> verseInfo;
        verseInfo.insert("surah", m_queryOpenDB.value(0).toInt());
        verseInfo.insert("ayah", m_queryOpenDB.value(1).toInt());
        viList.append(verseInfo);
        verseInfo.clear();
    }

    return viList;
}

QString DBManager::getSurahName(const int sIdx, bool en)
{
    m_queryOpenDB = QSqlQuery(m_openDBCon);

    if (en) {
        m_queryOpenDB.prepare("SELECT sura_name_en FROM verses WHERE sura_no=:i");
    } else {
        m_queryOpenDB.prepare("SELECT sura_name_ar FROM verses WHERE sura_no=:i");
    }

    m_queryOpenDB.bindValue(0, sIdx);
    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }

    m_queryOpenDB.next();
    return m_queryOpenDB.value(0).toString();
}

QString DBManager::getSurahNameGlyph(const int sura)
{
    m_queryOpenDB = QSqlQuery(m_openDBCon);

    m_queryOpenDB.prepare("SELECT glyph FROM surah_symbols WHERE surah=:i");
    m_queryOpenDB.bindValue(0, sura);
    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }

    m_queryOpenDB.next();
    return m_queryOpenDB.value(0).toString();
}

QString DBManager::getJuzGlyph(const int juz)
{
    m_queryOpenDB = QSqlQuery(m_openDBCon);

    m_queryOpenDB.prepare("SELECT juzz_glyph FROM juzz_symbols WHERE juzz=:j");
    m_queryOpenDB.bindValue(0, juz);
    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }

    m_queryOpenDB.next();

    return m_queryOpenDB.value(0).toString();
}

int DBManager::getSurahStartPage(int surahIdx)
{
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT page FROM verses WHERE sura_no=:sn AND aya_no=1");

    m_queryOpenDB.bindValue(0, surahIdx);

    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }
    m_queryOpenDB.next();

    return m_queryOpenDB.value(0).toInt();
}

int DBManager::getVersePage(const int &surahIdx, const int &verse)
{
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT page FROM verses WHERE sura_no=:s AND aya_no=:v");
    m_queryOpenDB.bindValue(0, surahIdx);
    m_queryOpenDB.bindValue(1, verse);

    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }
    m_queryOpenDB.next();

    return m_queryOpenDB.value(0).toInt();
}

QString DBManager::getVerseText(const int sIdx, const int vIdx)
{
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT aya_text FROM verses WHERE sura_no=:s AND aya_no=:v");
    m_queryOpenDB.bindValue(0, sIdx);
    m_queryOpenDB.bindValue(1, vIdx);

    if (!m_queryOpenDB.exec()) {
        qCritical() << "Error occurred during getVerseInfo SQL statment exec";
    }
    m_queryOpenDB.next();

    return m_queryOpenDB.value(0).toString();
}

QList<QMap<int, QString>> DBManager::searchVerses(QString searchText)
{
    // 0 -> surah idx with name matching search text / 1 -> verse with matching text
    QList<QMap<int, QString>> results;
    m_queryOpenDB = QSqlQuery(m_openDBCon);
    m_queryOpenDB.prepare("SELECT sura_no FROM verses WHERE sura_name_ar like '%:searchText%'");
    m_queryOpenDB.bindValue(0, searchText);
    if (!m_queryOpenDB.exec()) {
        qCritical() << "[CRITICAL] Error occurred during searchVerses 1st SQL statment exec";
    }

    while (m_queryOpenDB.next()) {
        QMap<int, QString> entry;
        entry.insert(0, m_queryOpenDB.value(0).toString());
        qInfo() << entry;
        results.append(entry);
    }

    m_queryOpenDB.clear();
    m_queryOpenDB.prepare("SELECT aya_text FROM verses WHERE aya_text_emlaey like '%:searchText%'");
    m_queryOpenDB.bindValue(0, searchText);
    if (!m_queryOpenDB.exec()) {
        qCritical() << "[CRITICAL] Error occurred during searchVerses 2nd SQL statment exec";
    }

    while (m_queryOpenDB.next()) {
        QMap<int, QString> entry;
        entry.insert(1, m_queryOpenDB.value(0).toString());
        qInfo() << entry;
        results.append(entry);
    }

    return results;
}

/* ---------------- Side content data ---------------- */

QString DBManager::getVerseGlyphs(const int sIdx, const int vIdx)
{
    m_queryOpenDB = QSqlQuery(m_openDBCon);

    m_queryOpenDB.prepare("SELECT code FROM ayah_glyphs WHERE surah=:su AND ayah=:v");
    m_queryOpenDB.bindValue(0, sIdx);
    m_queryOpenDB.bindValue(1, vIdx);

    if (!m_queryOpenDB.exec())
        qFatal("[WARNING] Couldn't execute getVerseGlyphs query!");

    m_queryOpenDB.next();

    return m_queryOpenDB.value(0).toString();
}

QString DBManager::getTafsir(const int sIdx, const int vIdx)
{
    setOpenDatabase(Database::tafsir, m_tafsirDbPath.filePath());

    m_queryOpenDB = QSqlQuery(m_openDBCon);
    QString q = "SELECT text FROM %0 WHERE sura=%1 AND aya=%2";
    q = q.arg(m_tafsirDbPath.baseName()).arg(sIdx).arg(vIdx);
    m_queryOpenDB.prepare(q);

    if (!m_queryOpenDB.exec()) {
        qFatal("[WARNING] Couldn't execute getTafsir query!");
    }
    qInfo() << m_tafsirDbPath.baseName();
    m_queryOpenDB.next();

    setOpenDatabase(Database::quran, m_quranDbPath.filePath());

    return m_queryOpenDB.value(0).toString();
}

QString DBManager::getTranslation(const int sIdx, const int vIdx)
{
    setOpenDatabase(Database::translation, m_transDbPath.filePath());

    m_queryOpenDB = QSqlQuery(m_openDBCon);
    QString q = "SELECT text FROM %0 WHERE sura=%1 AND aya=%2";
    q = q.arg(m_transDbPath.baseName()).arg(sIdx).arg(vIdx);
    m_queryOpenDB.prepare(q);

    if (!m_queryOpenDB.exec())
        qFatal("[WARNING] Couldn't execute getTrans query!");

    m_queryOpenDB.next();

    setOpenDatabase(Database::quran, m_quranDbPath.filePath());

    return m_queryOpenDB.value(0).toString();
}
