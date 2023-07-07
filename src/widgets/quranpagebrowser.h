#ifndef QURANPAGEBROWSER_H
#define QURANPAGEBROWSER_H

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
    QuranPageBrowser(QWidget *parent = nullptr,
                     int qcfVersion = 1,
                     int initPage = 1,
                     DBManager *dbPtr = nullptr,
                     QSettings *appSettings = nullptr,
                     const QString &iconsPath = ":/resources/light/");

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
    void contextMenuEvent(QContextMenuEvent *event) override;
#endif

private slots:
    void actionZoomIn();
    void actionZoomOut();
    void actionCopy();

private:
    QString &justifyHeader(QString &baseHeader);
    QSize calcPageLineSize(QStringList &lines);
    QImage surahFrame(int surah);
    bool m_darkMode;
    int m_page = -1;
    int m_qcfVer;
    int m_fontSize;
    int m_highlightedIdx = -1;
    QSize m_pageLineSize;
    QStringList m_currPageLines;
    QString m_currPageHeader;
    QPoint m_mousePos;
    QPoint m_mouseGlobalPos;
    QString m_resourcePath;
    QString m_pageFont;
    QString m_fontPrefix;
    QString m_bsmlFont;
    QAction *m_zoomIn;
    QAction *m_zoomOut;
    QAction *m_copyAct;
    QAction *m_selectAct;
    QAction *m_playAct;
    QAction *m_tafsirAct;
    QAction *m_actAddBookmark;
    QAction *m_actRemBookmark;
    QTextCursor *m_highlighter;
    QTextBlockFormat m_pageFormat;
    QTextCharFormat m_headerTextFormat;
    QTextCharFormat m_bodyTextFormat;
    QBrush m_highlightColor;
    QList<int *> m_pageVerseCoords;
    QMap<QString, QString> m_easternNumsMap;
    DBManager *m_dbMgr;
    QSettings *m_settingsPtr;
};

#endif // QURANPAGEBROWSER_H
