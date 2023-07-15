#ifndef QURANPAGEBROWSER_H
#define QURANPAGEBROWSER_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QTextBrowser>
#include <QTextCursor>

class QuranPageBrowser : public QTextBrowser
{
  Q_OBJECT

public:
  QuranPageBrowser(QWidget* parent = nullptr,
                   DBManager* dbMgr = nullptr,
                   int initPage = 1);

  void createActions();
  void updateFontSize();
  QString getEasternNum(QString num);
  QString pageHeader(int page);
  void constructPage(int pageNo, bool manualSz = false);
  void highlightVerse(int verseIdxInPage);
  int lmbVerseMenu(bool favoriteVerse);
  int bestFitFontSize();

  int fontSize() const;
  QString pageFont() const;

signals:
  void copyVerse(int IdxInPage);

protected:
#ifndef QT_NO_CONTEXTMENU
  void contextMenuEvent(QContextMenuEvent* event) override;
#endif

private slots:
  void actionZoomIn();
  void actionZoomOut();

private:
  QSettings* const m_settings = g_settings;
  const QDir& m_resources = g_themeResources;
  const QString& m_bsmlFont = g_qcfBSMLFont;
  const QString& m_fontnamePrefix = g_qcfFontPrefix;
  const int m_qcfVer = g_qcfVersion;
  const bool m_darkMode = g_darkMode;
  QString& justifyHeader(QString& baseHeader);
  QSize calcPageLineSize(QStringList& lines);
  QImage surahFrame(int surah);
  int m_page = -1;
  int m_fontSize;
  int m_highlightedIdx = -1;
  QSize m_pageLineSize;
  QStringList m_currPageLines;
  QString m_currPageHeader;
  QPoint m_mousePos;
  QPoint m_mouseGlobalPos;
  QString m_pageFont;
  QAction* m_zoomIn;
  QAction* m_zoomOut;
  QAction* m_copyAct;
  QAction* m_selectAct;
  QAction* m_playAct;
  QAction* m_tafsirAct;
  QAction* m_actAddBookmark;
  QAction* m_actRemBookmark;
  QTextCursor* m_highlighter;
  QTextBlockFormat m_pageFormat;
  QTextCharFormat m_headerTextFormat;
  QTextCharFormat m_bodyTextFormat;
  QBrush m_highlightColor;
  QList<int*> m_pageVerseCoords;
  QMap<QString, QString> m_easternNumsMap;
  DBManager* m_dbMgr;
};

#endif // QURANPAGEBROWSER_H
