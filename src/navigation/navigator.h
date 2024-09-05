#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QObject>
#include <QPointer>
#include <navigation/verseobserver.h>
#include <service/quranservice.h>
#include <types/verse.h>

/**
 * @class Navigator
 * @brief Singleton class responsible for navigating and managing verses in the
 * Quran.
 *
 * This class provides functionality for navigating to specific verses, surahs,
 * pages, and juz. It also manages a list of observers that are notified
 * whenever the current verse changes.
 */
class Navigator : public QObject
{
  Q_OBJECT

public:
  /**
   * @brief Gets the singleton instance of the Navigator class.
   * @return The singleton instance of Navigator.
   */
  static Navigator& getInstance();
  /**
   * @brief Adds an observer to be notified of verse changes.
   * @param observer Pointer to the VerseObserver to be added.
   */
  void addObserver(VerseObserver* observer);
  /**
   * @brief Removes an observer from the list of observers.
   * @param observer Pointer to the VerseObserver to be removed.
   */
  void removeObserver(VerseObserver* observer);
  /**
   * @brief Notifies all registered observers of a verse change.
   */
  void notifyObservers();
  /**
   * @brief Navigates to a specific verse.
   * @param verse The Verse object representing the target verse.
   */
  void navigateToVerse(const Verse& verse);
  /**
   * @brief Navigates to the first verse of a specific surah.
   * @param surah The surah number to navigate to.
   */
  void navigateToSurah(int surah);
  /**
   * @brief Navigates to the first verse on a specific page.
   * @param page The page number to navigate to.
   */
  void navigateToPage(int page);
  /**
   * @brief Navigates to the first verse of a specific juz.
   * @param juz The juz number to navigate to.
   */
  void navigateToJuz(int juz);
  /**
   * @brief Navigates to the next verse in the Quran.
   * If the current verse is the last verse of the Quran, no navigation occurs.
   */
  void navigateToNextVerse();
  /**
   * @brief Navigates to the previous verse in the Quran.
   * If the current verse is the first verse of the Quran, no navigation occurs.
   */
  void navigateToPreviousVerse();
  /**
   * @brief Navigates to the next surah in the Quran.
   * If the current surah is the last surah, no navigation occurs.
   */
  void navigateToNextSurah();
  /**
   * @brief Navigates to the previous surah in the Quran.
   * If the current surah is the first surah, no navigation occurs.
   */
  void navigateToPreviousSurah();
  /**
   * @brief Navigates to the next juz in the Quran.
   * If the current juz is the last juz, no navigation occurs.
   */
  void navigateToNextJuz();
  /**
   * @brief Navigates to the previous juz in the Quran.
   * If the current juz is the first juz, no navigation occurs.
   */
  void navigateToPreviousJuz();

private:
  Navigator();        ///< Private constructor for singleton pattern.
  Verse& m_currVerse; ///< Reference to the current verse being displayed.
  const QuranService* m_quranService; ///< Pointer to the Quran service for
                                      ///< accessing verse information.
  QList<VerseObserver*>
    m_verseObservers; ///< List of observers to be notified of verse changes.
};

#endif // NAVIGATOR_H
