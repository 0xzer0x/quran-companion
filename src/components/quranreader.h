#ifndef QURANREADER_H
#define QURANREADER_H

#include "navigator.h"

#include <QLabel>
#include <QScrollArea>
#include <QWidget>
#include <interfaces/verseobserver.h>
#include <player/playbackcontroller.h>
#include <player/verseplayer.h>
#include <repository/bookmarksrepository.h>
#include <repository/tafsirrepository.h>
#include <repository/translationrepository.h>
#include <service/bookmarkservice.h>
#include <service/glyphservice.h>
#include <service/quranservice.h>
#include <service/tafsirservice.h>
#include <service/translationservice.h>
#include <types/verse.h>
#include <widgets/quranpagebrowser.h>
#include <widgets/verseframe.h>
typedef Configuration::ReaderMode ReaderMode;

namespace Ui {
class QuranReader;
}

/**
 * @class QuranReader
 * @brief The QuranReader class is responsible for displaying the Quran page(s)
 * and the side content
 */
class QuranReader
  : public QWidget
  , public VerseObserver
{
  Q_OBJECT

public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   * @param player - pointer to VersePlayer instance
   */
  explicit QuranReader(QWidget* parent,
                       QPointer<PlaybackController> playbackController);
  ~QuranReader();

  void activeVerseChanged();

public slots:
  /**
   * @brief highlight the currently active Verse m_currVerse in the
   * active QuranPageBrowser and the side panel depending on the ::ReaderMode
   */
  void highlightCurrentVerse();
  /**
   * @brief highlight the currently active VerseFrame
   */
  void setHighlightedFrame();
  /**
   * @brief toggle the main reader view by hiding one of the panels
   */
  void toggleReaderView();
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
   * @brief ensure the page given is visible and update other members to match
   * the properties of the first verse in the page, calls the appropriate
   * navigation function according to the ::ReaderMode
   * @param page - page to navigate to
   * @param updateElements - boolean flag to indicate whether to update other
   * elements or not
   * @param automaticFlip - boolean indicating whether the function was called
   * by internal signal to automatically flip the page
   */
  void gotoPage(int page);
  /**
   * @brief slot for updating the page font size of all quran pages
   */
  void updatePageFontSize();

signals:
  void showBetaqa(int surah);
  void copyVerseText(const Verse& v);
  void showVerseTafsir(const Verse& v);
  void showVerseTranslation(const Verse& v);
  void showVerseThoughts(const Verse& v);

private slots:
  /**
   * @brief callback function for clicking verses in the QuranPageBrowser that
   * takes actions based on the chosen option in the menu
   * @param hrefUrl - "#idx" where idx is the verse index relative to the start
   * of the page (=index in the page Verse QList)
   */
  void verseAnchorClicked(const QUrl& hrefUrl);
  /**
   * @brief slot to navigate to the clicked verse in the side panel and update
   * UI elements
   */
  void verseClicked();
  /**
   * @brief single page mode navigation
   */
  void gotoSinglePage();
  /**
   * @brief double page mode navigation
   * @param page - page to navigate to
   */
  void gotoDoublePage(int page);

private:
  Ui::QuranReader* ui;
  /**
   * @brief reference to the shared current verse instance
   */
  Verse& m_currVerse;
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;

  Navigator& m_navigator;

  /**
   * @brief reference to the singleton TranslationRepository instance
   */
  TranslationService* m_translationService;
  /**
   * @brief reference to the singleton BookmarksRepository instance
   */
  BookmarkService* m_bookmarkService;
  /**
   * @brief reference to the singleton QuranRepository instance
   */
  QuranService* m_quranService;
  /**
   * @brief reference to the singleton GlyphsRepository instance
   */
  const GlyphService* m_glyphService;
  /**
   * @brief connects signals and slots for different UI components and
   * shortcuts
   */
  void setupConnections();
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
   * @brief updates the list that containsVerse instances for verses in the
   * current page
   */
  void updatePageVerseInfoList();
  /**
   * @brief QScrollArea used in single page mode to display verses &
   * translation
   */
  QPointer<QScrollArea> m_scrlVerseByVerse;
  /**
   * @brief pointer to currently active QuranPageBrowser instance, must be one
   * of the values in m_quranBrowsers array
   */
  QPointer<QuranPageBrowser> m_activeQuranBrowser;
  /**
   * @brief array of QuranPageBrowser instances used in different modes, index 0
   * is used in both modes
   */
  QPointer<QuranPageBrowser> m_quranBrowsers[2];
  /**
   * @brief QList of QFrame pointers to VerseFrame elements in the single page
   * mode side panel
   */
  QList<QPointer<QFrame>> m_verseFrameList;
  /**
   * @brief pointer to the currently active page Verse list
   */
  const QList<Verse>* m_activeVList;
  /**
   * @brief array of 2 QLists of Verse instances for the verses in the
   * displayed page(s), index 0 is used in both reader modes
   */
  QList<Verse> m_vLists[2];
  /**
   * @brief pointer to the currently highlighted VerseFrame in the side panel
   */
  QPointer<VerseFrame> m_highlightedFrm;
  /**
   * @brief QFont used in the side panel translation
   */
  QFont m_sideFont;
  /**
   * @brief QFont used in displaying Quranic verse
   */
  QFont m_versesFont;

  QPointer<PlaybackController> m_playbackController;
};

#endif // QURANREADER_H
