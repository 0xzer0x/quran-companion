#ifndef QURANREADER_H
#define QURANREADER_H

#include "../globals.h"
#include "../utils/shortcuthandler.h"
#include "../utils/verse.h"
#include "../utils/verseplayer.h"
#include "../widgets/quranpagebrowser.h"
#include "../widgets/verseframe.h"
#include <QLabel>
#include <QScrollArea>
#include <QWidget>

namespace Ui {
class QuranReader;
}

class QuranReader : public QWidget
{
  Q_OBJECT

public:
  explicit QuranReader(QWidget* parent,
                       VersePlayer* player,
                       ShortcutHandler* handler);
  ~QuranReader();

public slots:
  void updatePageFontSize();
  /**
   * @brief highlight the currently active ::Verse m_currVerse in the
   * active QuranPageBrowser and the side panel depending on the ::ReaderMode
   */
  void highlightCurrentVerse();
  /**
   * @brief highlight the currently active VerseFrame
   */
  void setHighlightedFrame();
  /**
   * @brief navigate to the given Verse and update UI elements accordingly
   * @param v - Verse to navigate to
   */
  void navigateToVerse(Verse v);
  /**
   * @brief toggle the main reader view by hiding one of the panels
   */
  void toggleReaderView();
  /**
   * @brief set tafsir to the one in the settings, update the selected db
   */
  void updateLoadedTafsir();
  /**
   * @brief set translation to the one in the settings, update the selected db
   */
  void updateLoadedTranslation();
  /**
   * @brief redraw the current Quran page
   * @param manualSz - boolean flag to force the use of the manually set
   * fontsize
   */
  void redrawQuranPage(bool manualSz = false);
  /**
   * @brief updates the side panel with the translation of the current page
   * verses
   */
  void addSideContent();
  /**
   * @brief set side content font to the one in the settings
   */
  void updateSideFont();
  /**
   * @brief Updates the type of the verses shown and reload the font family and
   * size
   */
  void updateVerseType();
  /**
   * @brief update the highlight layer (fg/bg) used by the Quran browser(s)
   */
  void updateHighlight();
  /**
   * @brief gets the page of the 1st verse in this surah, moves to that page,
   * and starts playback of the surah
   * @param surahIdx - surah number (1-114)
   */
  void gotoSurah(int surahIdx);
  /**
   * @brief ensure the page given is visible and update other members to match
   * the properties of the first verse in the page, calls the appropriate
   * navigation function according to the ::ReaderMode
   * @param page - page to navigate to
   * @param updateElements - boolean flag to indicate whether to update other
   * elements or not
   * @param automaticFlip - boolean indicating whether the function was called
   * by internal signal to automatically flip the page
   */
  void gotoPage(int page,
                bool updateElements = true,
                bool automaticFlip = false);
  /**
   * @brief sets m_currVerse to the first verse in m_vInfoList
   */
  void setVerseToStartOfPage();

signals:
  void showBetaqa(int surah);
  void showVerseTafsir(const Verse& v);
  void copyVerseText(const Verse& v);
  void currentVerseChanged();
  void currentSurahChanged();
  void notifyBookmarkAdded();
  void notifyBookmarkRemoved();

private slots:
  /**
   * @brief callback function for clicking verses in the QuranPageBrowser that
   * takes actions based on the chosen option in the menu
   * @param hrefUrl - "#idx" where idx is the verse index relative to the start
   * of the page (=index in the page ::Verse QList)
   */
  void verseAnchorClicked(const QUrl& hrefUrl);
  /**
   * @brief slot to navigate to the clicked verse in the side panel and update
   * UI elements
   */
  void verseClicked();
  /**
   * @brief navigates to the next page relative to the current page
   */
  void nextPage(int step = 1);
  /**
   * @brief navigates to the previous page relative to the current page
   */
  void prevPage(int step = 1);
  /**
   * @brief single page mode navigation
   * @param page - page to navigate to
   */
  void gotoSinglePage(int page);
  /**
   * @brief double page mode navigation
   * @param page - page to navigate to
   */
  void gotoDoublePage(int page);

private:
  Verse* m_currVerse = Verse::current();
  QSettings* const m_settings = Globals::settings;
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  const QList<Tafsir>& m_tafasirList = Globals::tafasirList;
  const QString& m_updateToolPath = Globals::updateToolPath;
  const ReaderMode& m_readerMode = Globals::readerMode;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  fa::QtAwesome* m_fa = Globals::awesome;
  Ui::QuranReader* ui;
  void setupConnections();
  void setupShortcuts(ShortcutHandler* handler);
  /**
   * @brief load icons for different UI elements
   */
  void loadIcons();
  /**
   * @brief setup the reader layout and create widgets according to the current
   * ::ReaderMode
   */
  void loadReader();
  /**
   * @brief utility function to check whether 2 pages are beside each other in
   * 2-page mode
   * @details 2 pages are considered neighbors if the right page is odd and the
   * left page is directly after the right page
   * @param page1 - the right side page
   * @param page2 - the left side page
   * @return boolean value indicating whether the given pages are neighbors
   */
  bool areNeighbors(int page1, int page2);
  /**
   * @brief 2-page mode utility to switch the active page & verse info list to
   * the opposite side of the current
   * @details the currently active page is recogonized through
   * m_activeQuranBrowser and m_activeVList pointers
   */
  void switchActivePage();
  /**
   * @brief flip the current page/2-pages to the next page/2-pages
   */
  void btnNextClicked();
  /**
   * @brief flip the current page/2-pages to the previous page/2-pages
   */
  void btnPrevClicked();
  /**
   * @brief selects one of the verses in the currently displayed page(s)
   * @param browserIdx - index of the QuranPageBrowser which contains the target
   * verse
   * @param IdxInPage - index of the verse relative to the start of the
   * page
   */
  void selectVerse(int browserIdx, int IdxInPage);
  /**
   * @brief updates the list that contains::Verse instances for verses in the
   * current page
   */
  void updatePageVerseInfoList();
  /**
   * @brief QScrollArea used in single page mode to display verses &
   * translation
   */
  QScrollArea* m_scrlVerseByVerse = nullptr;
  /**
   * @brief pointer to currently active QuranPageBrowser instance, must be one
   * of the values in m_quranBrowsers array
   */
  QuranPageBrowser* m_activeQuranBrowser = nullptr;
  /**
   * @brief array of QuranPageBrowser instances used in different modes, index 0
   * is used in both modes
   */
  QuranPageBrowser* m_quranBrowsers[2]{};
  /**
   * @brief QList of QFrame pointers to VerseFrame elements in the single page
   * mode side panel
   */
  QList<QFrame*> m_verseFrameList;
  /**
   * @brief pointer to the currently active page ::Verse list
   */
  const QList<Verse>* m_activeVList;
  /**
   * @brief array of 2 QLists of ::Verse instances for the verses in the
   * displayed page(s), index 0 is used in both reader modes
   */
  QList<Verse> m_vLists[2];
  /**
   * @brief pointer to the currently highlighted VerseFrame in the side panel
   */
  VerseFrame* m_highlightedFrm = nullptr;
  VersePlayer* m_player = nullptr;
  /**
   * @brief QFont used in the side panel translation
   */
  QFont m_sideFont;
  /**
   * @brief QFont used in displaying Quranic verse
   */
  QFont m_versesFont;
};

#endif // QURANREADER_H
