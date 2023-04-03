#include "quranpagebrowser.h"

QuranPageBrowser::QuranPageBrowser(
    QWidget *parent, int qcfVersion, int initPage, DBManager *dbPtr, QSettings *appSettings)
    : QTextBrowser(parent)
    , m_qcfVer{qcfVersion}
    , m_dbPtr{dbPtr}
    , m_settingsPtr{appSettings}
    , m_highlighter{new QTextCursor(document())}
{
    m_highlightColor = QBrush(QColor(0, 161, 185));
    m_fontSize = m_settingsPtr->value("Reader/QuranFontSize", 22).toInt();
    m_darkMode = m_settingsPtr->value("Theme").toInt() == 1;
    setStyleSheet("QTextBrowser{background-color: transparent;}");

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

    constructPage(initPage);
}

void QuranPageBrowser::constructPage(int pageNo)
{
    if (!m_pageVerseCoords.empty()) {
        qDeleteAll(m_pageVerseCoords);
        m_pageVerseCoords.clear();
    }

    this->document()->clear();
    QTextCursor cur(this->document());

    QString bsmlFont = m_qcfVer == 1 ? "QCF_BSML" : "QCF2BSML";
    m_pageFont = m_qcfVer == 1 ? "QCF_P" : "QCF2";
    m_pageFont.append(QString::number(pageNo).rightJustified(3, '0'));

    int fontSize = pageNo < 3 ? m_fontSize + 5 : m_fontSize;

    QList<int> headerData = m_dbPtr->getPageMetadata(pageNo);

    QString suraHeader, jozzHeader;
    suraHeader.append("ﮌ");
    suraHeader.append(m_dbPtr->getSurahNameGlyph(headerData.at(0)));
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
    jozzHeader.append(m_dbPtr->getJuzGlyph(headerData.at(1)));

    // create a qlist of page lines
    QStringList lines = m_dbPtr->getPageLines(pageNo);

    int counter = 0, prevAnchor = pageNo < 3 ? 3 : 24;

    QTextBlockFormat pageFormat;
    pageFormat.setAlignment(Qt::AlignCenter);
    pageFormat.setNonBreakableLines(true);
    pageFormat.setLayoutDirection(Qt::RightToLeft);

    QTextCharFormat bsmlFormat, pageTextFormat;
    bsmlFormat.setFont(QFont(bsmlFont, fontSize - 6));
    pageTextFormat.setFont(QFont(m_pageFont, fontSize));
    if (pageNo > 2) {
        cur.insertBlock(pageFormat, bsmlFormat);
        cur.insertText(suraHeader + jozzHeader);
    }

    QFontMetrics fm(QFont(m_pageFont, fontSize));
    QString measureLine;
    if (pageNo < 3) {
        measureLine = lines.at(3);
    } else if (pageNo == 602 || pageNo == 604) {
        measureLine = lines.at(2);
    } else {
        measureLine = lines.last();
    }

    this->document()->setTextWidth(fm.size(Qt::TextSingleLine, measureLine.remove(':')).width() + 5);
    bool newSurah = false;
    foreach (QString l, lines) {
        l = l.trimmed();
        if (l.contains("frame")) {
            QImage frm(":/assets/images/sura_box.png"); // load the empty frame

            // construct the text to be put inside the frame
            QString frmText;
            frmText.append("ﰦ");
            frmText.append("ﮌ");
            frmText.append(m_dbPtr->getSurahNameGlyph(l.split('_').at(1).toInt()));

            // draw on top of the image the surah name text
            QPainter p(&frm);
            p.setPen(QPen(Qt::black));
            p.setFont(QFont(bsmlFont, 77));
            p.drawText(frm.rect(), Qt::AlignCenter, frmText);

            if (m_darkMode)
                frm.invertPixels();

            // insert the surah image in the document
            cur.insertBlock(pageFormat, pageTextFormat);
            cur.insertImage(
                frm.scaledToWidth(this->document()->textWidth(), Qt::SmoothTransformation));

            newSurah = true;
        } else {
            if (newSurah && pageNo != 1 && pageNo != 187) {
                newSurah = false;
                cur.insertBlock(pageFormat, bsmlFormat);
                cur.insertText("321");
            }

            cur.insertBlock(pageFormat, pageTextFormat);
            if (l.contains(':')) {
                foreach (QChar glyph, l) {
                    if (glyph != ':') {
                        cur.insertText(glyph);
                    } else {
                        QTextCharFormat anchorFormat;
                        anchorFormat.setAnchor(true);
                        anchorFormat.setAnchorHref("#" + QString::number(counter));

                        int lastInsertPos = cur.position();
                        cur.setPosition(prevAnchor, QTextCursor::KeepAnchor);
                        cur.mergeCharFormat(anchorFormat);

                        int *coords = new int[2];
                        coords[0] = prevAnchor;
                        coords[1] = lastInsertPos;
                        m_pageVerseCoords.append(coords);

                        counter++;
                        prevAnchor = lastInsertPos;
                        cur.setPosition(lastInsertPos);
                    }
                }

            } else {
                cur.insertText(l);
            }
        }
    }

    pageTextFormat.setFont(QFont("Amiri", m_fontSize - 4));
    cur.insertBlock(pageFormat, pageTextFormat);
    cur.insertText(getEasternNum(QString::number(pageNo)));

    setAlignment(Qt::AlignCenter);
}

void QuranPageBrowser::highlightVerse(int verseIdxInPage)
{
    if (verseIdxInPage > m_pageVerseCoords.size() || verseIdxInPage == -1) {
        qCritical() << "verseIdxInPage is out of page coords range!!!";
        return;
    }

    QTextCharFormat tcf;
    tcf.setForeground(m_darkMode ? Qt::white : Qt::black);

    if (m_highlighter->hasSelection())
        m_highlighter->mergeCharFormat(tcf); // de-highlight any previous highlights

    tcf.setForeground(m_highlightColor);

    const int *const bounds = m_pageVerseCoords.at(verseIdxInPage);
    qInfo("%d %d", bounds[0], bounds[1]);

    m_highlighter->setPosition(bounds[0]);
    m_highlighter->setPosition(bounds[1], QTextCursor::KeepAnchor);
    m_highlighter->mergeCharFormat(tcf);

    qInfo() << "Selection start:" << m_highlighter->selectionStart()
            << " Selection end:" << m_highlighter->selectionEnd();
}

QString QuranPageBrowser::getEasternNum(QString num)
{
    QString easternNum;

    for (int i = 0; i < num.size(); i++) {
        easternNum.append(m_easternNumsMap.value(num[i]));
    }
    return easternNum;
}

void QuranPageBrowser::setFontSize(int newFontSize)
{
    m_fontSize = newFontSize;
}

int QuranPageBrowser::fontSize() const
{
    return m_fontSize;
}

QString QuranPageBrowser::pageFont() const
{
    return m_pageFont;
}
