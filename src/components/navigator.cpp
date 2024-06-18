#include "navigator.h"
#include <utils/servicefactory.h>

Navigator::Navigator()
  : QObject()
  , m_currVerse(Verse::getCurrent())
  , m_quranService(ServiceFactory::quranService())
{
}

Navigator&
Navigator::getInstance()
{
  static Navigator navigator;
  return navigator;
}

void
Navigator::addObserver(VerseObserver* observer)
{
  m_verseObservers.append(observer);
}

void
Navigator::removeObserver(VerseObserver* observer)
{
  m_verseObservers.removeAll(observer);
}

void
Navigator::notifyObservers()
{
  for (VerseObserver* observer : m_verseObservers)
    observer->activeVerseChanged();
}

void
Navigator::navigateToVerse(const Verse& verse)
{
  m_currVerse.update(verse);
  notifyObservers();
}

void
Navigator::navigateToSurah(int surah)
{
  Verse start(m_quranService->versePage(surah, 1),
              surah,
              (surah == 1 || surah == 9) ? 1 : 0);
  navigateToVerse(start);
}

void
Navigator::navigateToPage(int page)
{
  Verse top = m_quranService->firstInPage(page);
  navigateToVerse(top);
}

void
Navigator::navigateToJuz(int juz)
{
  Verse juzStart = m_quranService->getJuzStart(juz);
  navigateToVerse(juzStart);
}

void
Navigator::navigateToNextVerse()
{
  if (m_currVerse.surah() == 114 && m_currVerse.number() == 6)
    return;

  navigateToVerse(m_quranService->next(m_currVerse, true));
}

void
Navigator::navigateToPreviousVerse()
{

  if (m_currVerse.surah() == 1 && m_currVerse.number() == 1)
    return;

  navigateToVerse(m_quranService->previous(m_currVerse, true));
}

void
Navigator::navigateToNextSurah()
{
  if (m_currVerse.surah() < 114)
    navigateToSurah(m_currVerse.surah() + 1);
}

void
Navigator::navigateToPreviousSurah()
{
  if (m_currVerse.surah() > 1)
    navigateToSurah(m_currVerse.surah() - 1);
}

void
Navigator::navigateToNextJuz()
{
  int juz = m_quranService->getVerseJuz(m_currVerse);
  if (juz < 30)
    navigateToJuz(juz + 1);
}

void
Navigator::navigateToPreviousJuz()
{
  int juz = m_quranService->getVerseJuz(m_currVerse);
  if (juz > 1)
    navigateToJuz(juz - 1);
}
