#ifndef QURANPAGEBROWSER_H
#define QURANPAGEBROWSER_H

#include <QContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QSettings>
#include <QShortcut>
#include <QTextBrowser>
#include <QTextCursor>
#include "../utils/dbmanager.h"

class QuranPageBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    QuranPageBrowser(QWidget *parent = nullptr,
                     int qcfVersion = 1,
                     int initPage = 1,
                     DBManager *dbPtr = nullptr,
                     QSettings *appSettings = nullptr,
                     const QString &iconsPath = ":/assets/images/light/");

    void createActions();
    void updateFontSize();
    QString getEasternNum(QString num);
    QString constructPageHeader(int page);
    void constructPage(int pageNo);
    void highlightVerse(int verseIdxInPage);
    int lmbVerseMenu(bool favoriteVerse);

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
    void actionAddToFav();
    void actionRemoveFromFav();

private:
    bool m_darkMode;
    int m_page;
    int m_qcfVer;
    int m_fontSize;
    int m_pageWidth;
    int m_highlightedIdx = -1;
    QPoint m_mousePos;
    QPoint m_mouseGlobalPos;
    QString m_iconsPath;
    QString m_pageFont;
    QString m_fontPrefix;
    QString m_bsmlFont;
    QAction *m_zoomIn;
    QAction *m_zoomOut;
    QAction *m_copyAct;
    QAction *m_selectAct;
    QAction *m_playAct;
    QAction *m_actAddBookmark;
    QAction *m_actRemBookmark;
    QTextCursor *m_highlighter;
    QBrush m_highlightColor;
    QList<int *> m_pageVerseCoords;
    QMap<QString, QString> m_easternNumsMap;
    DBManager *m_dbPtr;
    QSettings *m_settingsPtr;
};

#endif // QURANPAGEBROWSER_H
