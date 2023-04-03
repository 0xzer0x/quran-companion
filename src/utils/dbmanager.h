#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QDebug>
#include <QDir>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

/*!
 * \class DBManager
 * 
 * \brief The DBManager class acts as an interface to preforming queries to different database files holding application data
 */
class DBManager : public QObject
{
    Q_OBJECT

public:
    explicit DBManager(QObject *parent = nullptr, int qcfVersion = 1);

    struct Verse
    {
        int page{1};
        int surah{1};
        int number{1};
    };

    struct Reciter
    {
        QString baseDirName{};
        QString displayName{};
        QString basmallahPath{};
        QString baseUrl{};
    };

    enum Database { quran, glyphs, tafsir, translation };
    enum Tafsir {
        muyassar,
        baghawy,
        e3rab,
        indonesian,
        katheer,
        qortoby,
        russian,
        sa3dy,
        tabary,
        tafheem,
        tanweer,
        waseet
    };
    enum Translation {
        ar_ma3any,
        bn_bengali,
        bs_korkut,
        de_bubenheim,
        en_sahih,
        es_navio,
        fr_hamidullah,
        ha_gumi,
        id_indonesian,
        it_piccardo,
        ku_asan,
        ml_abdulhameed,
        ms_basmeih,
        nl_siregar,
        pr_tagi,
        pt_elhayek,
        ru_kuliev,
        so_abduh,
        sq_nahi,
        sv_bernstrom,
        sw_barwani,
        ta_tamil,
        th_thai,
        tr_diyanet,
        ur_jalandhry,
        uz_sodik,
        zh_jian
    };

    void setCurrentTafsir(Tafsir tafsirName);
    void setCurrentTranslation(Translation translationName);
    void setOpenDatabase(Database db, QString filePath);

    int getSurahStartPage(int surahIdx);
    int getSurahVerseCount(const int surahIdx);
    QList<int> getPageMetadata(const int page);
    QStringList getPageLines(const int page);

    int getVersePage(const int &surahIdx, const int &verse);
    QList<int> getVerseBounds(const int surah, const int ayah);
    QList<QString> getVersesInPage(const int page);
    QList<Verse> getVerseInfoList(const int page);

    QString getSurahName(const int sIdx, bool en = true);
    QString getSurahNameGlyph(const int sura);
    QString getJuzGlyph(const int juz);
    QString getVerseText(const int sIdx, const int vIdx);
    QString getVerseGlyphs(const int sIdx, const int vIdx);
    QString getTafsir(const int sIdx, const int vIdx);
    QString getTranslation(const int sIdx, const int vIdx);
    QList<Verse> searchVerses(QString searchText);
    QList<int> searchSurahs(QString searchText);

private:
    int m_qcfVer;
    QSqlDatabase m_openDBCon;
    QSqlQuery m_queryOpenDB;
    QDir m_dbDir = QDir::currentPath() + QDir::separator() + "assets";
    Tafsir m_currTafsir = Tafsir::sa3dy;
    Translation m_currTrans = Translation::en_sahih;
    QString m_tafsirDbFilename;
    QString m_transDbFilename;
    QFileInfo m_tafsirDbPath;
    QFileInfo m_transDbPath;
    QFileInfo m_quranDbPath;
    QFileInfo m_glyphsDbPath;
};

#endif // DBMANAGER_H
