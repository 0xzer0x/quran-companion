#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QObject>
#include <QPointer>
#include <interfaces/verseobserver.h>
#include <service/quranservice.h>
#include <types/navigationinitiator.h>
#include <types/verse.h>

class Navigator : public QObject
{
  Q_OBJECT
public:
  static Navigator& getInstance();

  void addObserver(VerseObserver* observer);
  void removeObserver(VerseObserver* observer);
  void notifyObservers();

  void navigateToVerse(const Verse& verse);
  void navigateToSurah(int surah);
  void navigateToPage(int page);
  void navigateToJuz(int juz);

  void navigateToNextVerse();
  void navigateToPreviousVerse();

  void navigateToNextSurah();
  void navigateToPreviousSurah();

  void navigateToNextJuz();
  void navigateToPreviousJuz();

private:
  Navigator();
  Verse& m_currVerse;
  const QuranService* m_quranService;
  QList<VerseObserver*> m_verseObservers;
};

#endif // NAVIGATOR_H
