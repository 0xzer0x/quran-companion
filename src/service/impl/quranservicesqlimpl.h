#ifndef QURANSERVICESQLIMPL_H
#define QURANSERVICESQLIMPL_H

#include <repository/quranrepository.h>
#include <service/quranservice.h>

class QuranServiceSqlImpl : public QuranService
{
private:
  QuranRepository& m_quranRepository;

public:
  QuranServiceSqlImpl();

  QPair<int, int> pageMetadata(const int page) const override;

  int getVersePage(const int& surahIdx, const int& verse) const override;

  int getJuzStartPage(const int juz) const override;

  int getJuzOfPage(const int page) const override;

  QList<QList<int>> verseInfoList(const int page) const override;

  QString verseText(const int sIdx, const int vIdx) const override;

  int surahStartPage(int surahIdx) const override;

  QString surahName(const int sIdx, bool ar) const override;

  QList<int> verseById(const int id) const override;

  int versePage(const int& surahIdx, const int& verse) const override;

  QList<int> searchSurahNames(QString text) const override;

  QList<QList<int>> searchSurahs(QString searchText,
                                 const QList<int> surahs,
                                 const bool whole) const override;

  QList<QList<int>> searchVerses(QString searchText,
                                 const int range[],
                                 const bool whole) const override;

  QList<int> randomVerse() const override;

  QStringList surahNames() const override;

  Verse next(const Verse& verse, bool withBasmallah) const override;

  Verse previous(const Verse& verse, bool withBasmallah) const override;
};

#endif // QURANSERVICESQLIMPL_H
