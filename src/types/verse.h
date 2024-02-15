#ifndef VERSE_H
#define VERSE_H

#include "../utils/settings.h"
#include "../utils/dbmanager.h"
#include <QSharedPointer>

/**
 * @brief Verse class represents a single quran verse
 * @details Quran verses consist of 3 attributes. page (1-604). surah (1-114).
 * number represents the number of the verse in the surah (0-surah verse count).
 * Basmallah before the 1st verse is represented as verse number 0.
 */
class Verse
{
public:
  static Verse* current();
  static QList<Verse> fromList(QList<QList<int>> lst);

  Verse();
  Verse(const QList<int> vInfo);
  Verse(const Verse& cp) = default;
  explicit Verse(int page, int surah, int number);

  QList<int> toList() const;
  void update(const Verse& v);
  void update(const QList<int>& vInfo);
  Verse next();
  Verse prev();

  Verse& operator=(const Verse& cp);
  Verse& operator=(const QList<int>& vInfo);
  bool operator==(const Verse& v2) const;
  bool operator!=(const Verse& v2) const;
  bool operator<(const Verse& v2) const;
  bool operator>(const Verse& v2) const;

  int page() const;
  int surah() const;
  int number() const;
  int surahCount() const;
  void setPage(int newPage);
  void setSurah(int newSurah);
  void setNumber(int newNumber);

private:
  const QSharedPointer<QSettings> m_settings = Settings::settings;
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  void updateSurahCount();

  int m_page = -1;      ///< verse page
  int m_surah = -1;     ///< verse surah number
  int m_number = -1;    ///< verse number in surah
  int m_surahCount = 0; ///< surah verse count
};

#endif // VERSE_H
