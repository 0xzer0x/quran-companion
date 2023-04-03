#ifndef PAGECONSTRUCTOR_H
#define PAGECONSTRUCTOR_H

#include <QDir>
#include <QObject>
#include <QSettings>
#include <QTextCursor>
#include "dbmanager.h"

/*!
 * \class PageConstructor
 * 
 * \brief The PageConstructor class generates quran pages using the QCF fonts from 
 * their glyph codes stored per-page in text files
 */
class PageConstructor : public QObject
{
    Q_OBJECT

public:
    explicit PageConstructor(QObject *parent = nullptr,
                             int qcfVersion = 1,
                             DBManager *dbPtr = nullptr,
                             QSettings *appSettings = nullptr);

    int currentPage() const;
    void constructDoc(int pageNum, bool darkMode);
    QString getEasternNum(QString num);
    QTextDocument *pageTextD() const;
    QTextCharFormat pFmt() const;
    QString pageFont() const;
    int fontSize() const;
    void setFontSize(int newFontSize);

private:
    int m_currentPage;
    int m_qcfVer;
    QString m_pageFont;
    int m_fontSize;
    QMap<QString, QString> m_easternNumsMap;
    QTextCharFormat m_pFmt;
    QTextBlockFormat m_pTbf;
    QTextDocument *m_pageTextD;
    DBManager *m_dataDB;
    QSettings *m_settingsPtr;
};

#endif // PAGECONSTRUCTOR_H
