#ifndef QURANPAGEBROWSER_H
#define QURANPAGEBROWSER_H

#include <QPainter>
#include <QSettings>
#include <QTextBrowser>
#include <QTextCursor>
#include "../utils/dbmanager.h"

class QuranPageBrowser : public QTextBrowser
{
public:
    QuranPageBrowser(QWidget *parent = nullptr,
                     int qcfVersion = 1,
                     int initPage = 1,
                     DBManager *dbPtr = nullptr,
                     QSettings *appSettings = nullptr);

    int fontSize() const;
    QString pageFont() const;
    void constructPage(int pageNo);
    QString getEasternNum(QString num);
    void setFontSize(int newFontSize);
    void highlightVerse(int verseIdxInPage);
    QString constructPageHeader(int page);

private:
    bool m_darkMode;
    int m_qcfVer;
    int m_fontSize;
    QString m_pageFont;
    QTextCursor *m_highlighter;
    QBrush m_highlightColor;
    QList<int *> m_pageVerseCoords;
    QMap<QString, QString> m_easternNumsMap;
    DBManager *m_dbPtr;
    QSettings *m_settingsPtr;
};

#endif // QURANPAGEBROWSER_H
