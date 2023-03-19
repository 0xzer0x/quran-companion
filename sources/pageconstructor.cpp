#include "../headers/pageconstructor.h"

#include <QApplication>
#include <QPainter>

/*!
 * \brief the PageConstructor class is responsible for constructing Quran pages to appear in the main window
 * \param parent the parent QObject
 * \param dbPtr a pointer to the database management interface to query the quran sqlite db
 */
PageConstructor::PageConstructor(QObject *parent, DBManager *dbPtr, QSettings *appSettings)
    : QObject{parent}
{
    // set the m_dataDB pointer to the DBManager object
    m_dataDB = dbPtr;
    m_pageTextD = new QTextDocument;
    m_settingsPtr = appSettings;
    m_fontSize = m_settingsPtr->value("QuranFontSize", 22).toInt();

    m_easternNumsMap.insert("0", "٠");
    m_easternNumsMap.insert("1", "١");
    m_easternNumsMap.insert("2", "٢");
    m_easternNumsMap.insert("3", "٣");
    m_easternNumsMap.insert("4", "٤");
    m_easternNumsMap.insert("5", "٥");
    m_easternNumsMap.insert("6", "٦");
    m_easternNumsMap.insert("7", "٧");
    m_easternNumsMap.insert("8", "٨");
    m_easternNumsMap.insert("9", "٩");
}

/*!
 * \brief PageConstructor::constructDoc Constructs the QTextDocument for a page using QCF fonts & glyph codes stored in a page_pageNum.txt
 * file & sets the generated QTextDocument in the corresponding private attribute
 * 
 * \param pageNum the page to construct
 */
void PageConstructor::constructDoc(int pageNum, bool darkMode)
{
    m_pageTextD->clear();
    m_currentPage = pageNum;

    // set the correct font for the page e.g QCF_P009
    m_pageFont = "QCF_P";
    if (pageNum < 10)
        m_pageFont.append("00");
    else if (pageNum < 100)
        m_pageFont.append("0");
    m_pageFont.append(QString::number(pageNum));

    int fontSize = pageNum < 3 ? m_fontSize + 5 : m_fontSize;

    QList<int> headerData = m_dataDB->getPageMetadata(m_currentPage);

    QString suraHeader, jozzHeader;
    suraHeader.append("ﮌ");
    suraHeader.append(m_dataDB->getSurahNameGlyph(headerData.at(0)));
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    suraHeader.append("ﰦ");
    // construct jozz part
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰦ");
    jozzHeader.append("ﰸ");
    jozzHeader.append(m_dataDB->getJuzGlyph(headerData.at(1)));

    // read qcf glyphs page
    QFile pageText(m_pageInfoDir.filePath("page_" + QString::number(pageNum) + ".txt"));
    if (!pageText.open(QIODevice::ReadOnly))
        return;

    // create a qlist of page lines
    QList<QByteArray> lines = pageText.readAll().trimmed().split('\n');

    m_pTbf.setAlignment(Qt::AlignCenter);
    m_pTbf.setNonBreakableLines(true);
    m_pTbf.setLayoutDirection(Qt::RightToLeft);

    bool newSurah = false;
    QTextCursor tc(m_pageTextD);

    if (pageNum > 2) {
        m_pFmt.setFont(QFont("QCF_BSML", fontSize - 5));

        tc.insertBlock(m_pTbf, m_pFmt);
        tc.insertText(suraHeader + jozzHeader);
    }

    m_pFmt.setFont(QFont(m_pageFont, fontSize));
    QFontMetrics fm(QFont(m_pageFont, fontSize));
    if (m_currentPage < 3) {
        m_pageTextD->setTextWidth(fm.size(Qt::TextSingleLine, lines.at(3)).width() + 5);
    } else {
        m_pageTextD->setTextWidth(fm.size(Qt::TextSingleLine, lines.last()).width() + 5);
    }
    foreach (QByteArray l, lines) {
        l = l.trimmed();
        // if 'frame' exists in the line, means a surah frame should be added on this line
        if (l.contains("frame")) {
            QImage frm(":/assets/images/sura_box.png"); // load the empty frame

            // construct the text to be put inside the frame
            QString frmText;
            frmText.append("ﰦ");
            frmText.append("ﮌ");
            frmText.append(m_dataDB->getSurahNameGlyph(l.split('_').at(1).toInt()));

            // draw on top of the image the surah name text
            QPainter p(&frm);
            p.setPen(QPen(Qt::black));
            p.setFont(QFont("QCF_BSML", 77));
            p.drawText(frm.rect(), Qt::AlignCenter, frmText);

            if (darkMode)
                frm.invertPixels();

            // insert the surah image in the document
            tc.insertBlock(m_pTbf, m_pFmt);
            tc.insertImage(frm.scaledToWidth(m_pageTextD->textWidth(), Qt::SmoothTransformation));
            newSurah = true; // change flag for the next verse to insert basmalah before itself
        } else {
            if (newSurah && pageNum != 1 && pageNum != 187) {
                // change font, insert basmalah text, revert font back to page font
                newSurah = false;
                m_pFmt.setFont(QFont("QCF_BSML", fontSize - 6));
                tc.insertBlock(m_pTbf, m_pFmt);
                tc.insertText("321");

                m_pFmt.setFont(QFont(m_pageFont, fontSize));
            }

            // insert the page line
            tc.insertBlock(m_pTbf, m_pFmt);
            tc.insertText(QString(l));
        }
    }
    m_pFmt.setFont(QFont("Amiri", 18));
    tc.insertBlock(m_pTbf, m_pFmt);
    tc.insertText(getEasternNum(QString::number(pageNum)));
}

QString PageConstructor::getEasternNum(QString num)
{
    QString easternNum;

    for (int i = 0; i < num.size(); i++) {
        easternNum.append(m_easternNumsMap.value(num[i]));
    }
    return easternNum;
}

void PageConstructor::setFontSize(int newFontSize)
{
    m_fontSize = newFontSize;
}

/* -------------------- Getters ----------------------- */

QString PageConstructor::pageFont() const
{
    return m_pageFont;
}

int PageConstructor::currentPage() const
{
    return m_currentPage;
}

QTextDocument *PageConstructor::pageTextD() const
{
  return m_pageTextD;
}

QTextCharFormat PageConstructor::pFmt() const
{
  return m_pFmt;
}

int PageConstructor::fontSize() const
{
  return m_fontSize;
}
