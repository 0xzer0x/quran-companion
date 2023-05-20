#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QDebug>
#include <QDir>
#include <QObject>
#include <QRandomGenerator>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

/*!
 * \class DBManager
 *
 * \brief The DBManager class acts as an interface to preforming queries to
 * different database files holding application data
 */
class DBManager : public QObject
{
  Q_OBJECT

public:
  explicit DBManager(QObject* parent = nullptr, QSettings* settings = nullptr);
  enum Database
  {
    null,
    quran,
    glyphs,
    bookmarks,
    tafsir,
    translation
  };
  enum Tafsir
  {
    baghawy,
    qortoby,
    sa3dy,
    tabary,
    waseet,
    e3rab,
    tanweer,
    katheer,
    indonesian,
    russian,
    tafheem
  };
  enum Translation
  {
    muyassar,
    ar_ma3any,
    bn_bengali,
    bs_korkut,
    de_bubenheim,
    en_khattab,
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

  struct Verse
  {
    int page{ -1 };
    int surah{ -1 };
    int number{ -1 };
    bool operator==(const Verse& v2)
    {
      return (this->number == v2.number && this->surah == v2.surah);
    }
    bool operator!=(const Verse& v2)
    {
      return (this->number != v2.number || this->surah != v2.surah);
    }
  };
  struct Reciter
  {
    QString baseDirName{};
    QString displayName{};
    QString basmallahPath{};
    QString baseUrl{};
  };

  QList<QString> surahNameList();
  void setCurrentTafsir(Tafsir tafsirName);
  void setCurrentTranslation(Translation translationName);
  void setOpenDatabase(Database db, QString filePath);

  QList<int> getPageMetadata(const int page);
  QStringList getPageLines(const int page);
  QList<Verse> getVerseInfoList(const int page);

  QString getSurahNameGlyph(const int sura);
  QString getJuzGlyph(const int juz);
  QString getVerseGlyphs(const int sIdx, const int vIdx);
  QString getVerseText(const int sIdx, const int vIdx);

  int getSurahVerseCount(const int surahIdx);
  int getSurahStartPage(int surahIdx);
  QString getSurahName(const int sIdx);
  int getVersePage(const int& surahIdx, const int& verse);
  int getJozzStartPage(const int jozz);
  int getJozzOfPage(const int page);

  QList<int> searchSurahNames(QString text);
  QList<Verse> searchSurahs(QString searchText,
                            const QList<int> surahs,
                            const bool whole = false);
  QList<Verse> searchVerses(QString searchText,
                            const int range[2] = new int[2]{ 1, 604 },
                            const bool whole = false);

  QString getTafsir(const int sIdx, const int vIdx);
  QString getTranslation(const int sIdx, const int vIdx);

  QPair<Verse, QString> randomVerse();
  QList<Verse> favoriteVerses();
  bool isBookmarked(Verse v);
  bool addBookmark(Verse v);
  bool removeBookmark(Verse v);

private:
  int m_qcfVer;
  Database m_currentDb = null;
  QSqlDatabase m_openDBCon;
  QDir m_dbDir = QDir::currentPath() + QDir::separator() + "assets";
  QSettings* m_settings;
  Tafsir m_currTafsir = Tafsir::sa3dy;
  Translation m_currTrans = Translation::en_sahih;
  QString m_tafsirDbFilename;
  QString m_transDbFilename;
  QFileInfo m_tafsirDbPath;
  QFileInfo m_transDbPath;
  QFileInfo m_quranDbPath;
  QFileInfo m_glyphsDbPath;
  QFileInfo m_bookmarksDbPath;
  QStringList m_surahNames;
};

#endif // DBMANAGER_H
