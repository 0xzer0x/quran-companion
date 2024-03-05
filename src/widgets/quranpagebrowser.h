/**
 * @file quranpagebrowser.h
 * @brief Header file for QuranPageBrowser
 */

#ifndef QURANPAGEBROWSER_H
#define QURANPAGEBROWSER_H

#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QPointer>
#include <QPushButton>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QTextBrowser>
#include <QTextCursor>
#include <database/glyphsdb.h>
#include <database/qurandb.h>
#include <utils/configuration.h>
#include <utils/stylemanager.h>

/**
 * @brief QuranPageBrowser class is a modified QTextBrowser for displaying a
 * Quran page as it is in the Madani Mushaf using QCF fonts
 */
class QuranPageBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  /**
   * @brief Action enum represents LMB menu actions
   */
  enum Action
  {
    Null,          ///< no action (default)
    Play,          ///< select the verse and start playback
    Select,        ///< only select the verse
    Tafsir,        ///< show the tafsir for the verse
    Translation,   ///< show the translation for the verse
    Thoughts,      ///< show user thoughts for the verse
    Copy,          ///< copy the verse text to clipboard
    AddBookmark,   ///< add the verse to bookmarks
    RemoveBookmark ///< remove the verse from bookmarks
  };
  /**
   * @brief class constructor
   * @param parent - ponter to parent widget
   * @param dbMgr - pointer to DBManager instance
   * @param initPage - inital page to load
   */
  QuranPageBrowser(QWidget* parent = nullptr, int initPage = 1);
  /**
   * @brief sets m_fontSize to the fontsize in the settings file
   */
  void updateFontSize();
  /**
   * @brief convert between latin and arabic number glyphs
   * @param num - QString of the number to convert
   * @return QString of the converted number
   */
  QString getEasternNum(QString num);
  /**
   * @brief generate the header line which contains the top verse surah name and
   * the current juz
   * @details the returned page header contains a '$' which needs to be replaced
   * with the correct amount of space for the header to fit with the page size
   * @param page - page number to generate header for
   * @return QString of the page header without spacing
   */
  QString pageHeader(int page);
  /**
   * @brief construct Quran page
   * @details the page construction process is done through:
   * (1) clear the QTextBrowser and the verse coordinates QList
   * (2) set the new page font, header, page lines and page line pixel size
   * (3) set minimum widget width to preseve the page display as expected
   * (4) insert page lines which could be ('frame', 'bsml' or normal line)
   * (5) in case the line contains a verse end/separator (':'), carefully
   * insert the glyphs and set the verse anchor tag href to '#N' where N is the
   * verse number relative to the start of the page
   * (6) set the start and end cursor positions for the verse
   * (7) insert page footer with the page number
   *
   * @param pageNo - page number to generate
   * @param forceCustomSize - boolean to force the use of
   */
  void constructPage(int pageNo, bool forceCustomSize = false);
  /**
   * @brief highlight the specified verse in the displayed page
   * @param verseIdxInPage - 0-based index of the verse relative to the start of
   * the page
   */
  void highlightVerse(int verseIdxInPage);
  void resetHighlight();
  /**
   * @brief show the main verse interaction menu and return number related to
   * the chosen action
   * @param favoriteVerse - true if verse is bookmarked, false otherwise
   * @return QuranPageBrowser::Action that was selected from the menu
   */
  Action lmbVerseMenu(bool favoriteVerse);
  /**
   * @brief guess the best fontsize for the quran page based on the height of
   * the parent widget
   * @return suggested fontsize for the page
   */
  int bestFitFontSize();
  /**
   * @brief getter for m_fontSize
   * @return fontsize for the current page
   */
  int fontSize() const;
  /**
   * @brief getter for m_pageFont
   * @return QString of the page QCF font name
   */
  QString pageFont() const;

  int page() const;

public slots:
  /**
   * @brief increment the fontsize by 1 and redraw the quran page
   */
  void actionZoomIn();
  /**
   * @brief decrement the fontsize by 1 and redraw the quran page
   */
  void actionZoomOut();
  /**
   * @brief update the boolean indicating foreground highlighting and
   * re-highlight the current verse
   */
  void updateHighlightLayer();

signals:
  void copyVerse(int IdxInPage);

protected:
#ifndef QT_NO_CONTEXTMENU
  void contextMenuEvent(QContextMenuEvent* event) override;
#endif

private:
  Configuration& m_config;
  StyleManager& m_styleMgr;
  const QuranDb& m_quranDb;
  const GlyphsDb& m_glyphsDb;
  /**
   * @brief utility for creating menu actions for interacting with the widget
   */
  void createActions();
  /**
   * @brief adjusts the header string according to the page width
   * @param baseHeader - header string
   * @return reference to the adjusted string
   */
  QString& justifyHeader(QString& baseHeader);
  /**
   * @brief calculate the approximate pixel size of the page line
   * @param lines - QStringList of page lines
   * @return QSize of a single page line
   */
  QSize calcPageLineSize(QStringList& lines);
  /**
   * @brief generate QImage for the frame containing the surah name to insert in
   * the page
   * @param surah - surah number
   * @return QImage of the surah frame
   */
  QImage surahFrame(int surah);
  /**
   * @brief utility to set the href url for the text from the current cursor
   * position to the position given
   * @param cursor - pointer to the current QTextCursor used for inserting text
   * @param to  - the position in document to stop at
   * @param url - url to set for the selected portion
   * @return int - the current cursor postion
   */
  int setHref(QTextCursor* cursor, int to, QString url);
  /**
   * @brief boolean indicating whether to highlight the foreground of the active
   * verse or not
   */
  bool m_fgHighlight;
  /**
   * @brief the currently loaded page
   */
  int m_page = -1;
  /**
   * @brief the font size used
   */
  int m_fontSize;
  /**
   * @brief 0-based index of the highlighted verse relative to the start of the
   * page
   */
  int m_highlightedIdx = -1;
  /**
   * @brief the average size of the line in the current page
   */
  QSize m_pageLineSize;
  /**
   * @brief QStringList of the page line glyphs
   */
  QStringList m_currPageLines;
  /**
   * @brief QString of the page header
   */
  QString m_currPageHeader;
  /**
   * @brief mouse position relative to the widget
   */
  QPoint m_mousePos;
  /**
   * @brief mouse position on screen
   */
  QPoint m_mouseGlobalPos;
  /**
   * @brief QString of page font
   */
  QString m_pageFont;
  /**
   * @brief QAction for zoom-in functionality
   */
  QPointer<QAction> m_actZoomIn;
  /**
   * @brief QAction for zoom-out functionality
   */
  QPointer<QAction> m_actZoomOut;
  /**
   * @brief QAction for copy functionality
   */
  QPointer<QAction> m_actCopy;
  /**
   * @brief QAction for verse selection functionality
   */
  QPointer<QAction> m_actSelect;
  /**
   * @brief QAction for verse playback functionality
   */
  QPointer<QAction> m_actPlay;
  /**
   * @brief QAction for showing tafsir functionality
   */
  QPointer<QAction> m_actTafsir;
  /**
   * @brief m_actTranslation
   *
   * MODIFIED
   */
  QPointer<QAction> m_actTranslation;
  /**
   * @brief m_actThoughts
   *
   * MODIFIED
   */
  QPointer<QAction> m_actThoughts;
  /**
   * @brief QAction for bookmark addition functionality
   */
  QPointer<QAction> m_actAddBookmark;
  /**
   * @brief QAction for bookmark removal functionality
   */
  QPointer<QAction> m_actRemBookmark;
  /**
   * @brief QTextCursor used in highlighting verses
   */
  QSharedPointer<QTextCursor> m_highlighter;
  /**
   * @brief page format properties used in inserting lines
   */
  QTextBlockFormat m_pageFormat;
  /**
   * @brief character format used for header font properties
   */
  QTextCharFormat m_headerTextFormat;
  /**
   * @brief character format used for main page text font properties
   */
  QTextCharFormat m_bodyTextFormat;
  /**
   * @brief QBrush used for changing highlighted verse foreground color
   */
  QBrush m_highlightColor;
  /**
   * @brief QList of integer arrays of start & end position for each verse in
   * the current page
   */
  QList<QPair<int, int>> m_verseCoordinates;
  QPair<int, int> m_headerData;
  /**
   * @brief Hash Table used for converting page number to arabic numbers
   */
  QHash<QString, QString> m_easternNumsMap;
};

#endif // QURANPAGEBROWSER_H
