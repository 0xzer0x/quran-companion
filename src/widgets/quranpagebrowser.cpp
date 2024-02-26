/**
 * @file quranpagebrowser.cpp
 * @brief Implementation file for QuranPageBrowser
 */

#include "quranpagebrowser.h"
#include <QApplication>
#include <QRegularExpression>
#include <QtAwesome.h>
#include <utils/fontmanager.h>
#include <utils/stylemanager.h>
using namespace fa;

QuranPageBrowser::QuranPageBrowser(QWidget* parent, int initPage)
  : QTextBrowser(parent)
  , m_highlighter(new QTextCursor(document()))
  , m_highlightColor(QBrush(qApp->palette().color(QPalette::Highlight)))
{
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByMouse);
  setStyleSheet("QTextBrowser{background-color: transparent;}");
  createActions();
  updateFontSize();

  m_pageFont = FontManager::pageFontname(initPage);
  m_pageFormat.setAlignment(Qt::AlignCenter);
  m_pageFormat.setNonBreakableLines(true);
  m_pageFormat.setLayoutDirection(Qt::RightToLeft);
  m_headerTextFormat.setFont(QFont("PakType Naskh Basic"));

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

void
QuranPageBrowser::updateFontSize()
{
  m_fontSize =
    m_settings->value("Reader/QCF" + QString::number(m_qcfVer) + "Size", 22)
      .toInt();
  highlightVerse(m_highlightedIdx);
}

QString
QuranPageBrowser::getEasternNum(QString num)
{
  QString easternNum;

  for (int i = 0; i < num.size(); i++) {
    easternNum.append(m_easternNumsMap.value(num[i]));
  }
  return easternNum;
}

QString&
QuranPageBrowser::justifyHeader(QString& baseHeader)
{
  int margin = 20;
  int spacePos = baseHeader.indexOf('$');
  baseHeader.remove('$');

  QFontMetrics headerSpacing(m_headerTextFormat.font());
  while (headerSpacing.size(Qt::TextSingleLine, baseHeader).width() <
         m_pageLineSize.width() - margin) {
    baseHeader.insert(spacePos, " ");
  }

  return baseHeader;
}

QSize
QuranPageBrowser::calcPageLineSize(QStringList& lines)
{
  QFontMetrics fm(QFont(m_pageFont, m_fontSize));
  QString measureLine;
  if (m_page < 3) {
    measureLine = lines.at(3);
  } else if (m_page >= 602 || m_page == 596) {
    measureLine = lines.at(2);
  } else {
    measureLine = lines.at(lines.size() - 2);
  }

  return fm.size(Qt::TextSingleLine, measureLine.remove(':')) + QSize(0, 5);
}

QImage
QuranPageBrowser::surahFrame(int surah)
{
  QImage baseImage(":/resources/sura_box.png"); // load the empty frame

  // construct the text to be put inside the frame
  QString frmText;
  frmText.append("ﰦ");
  frmText.append("ﮌ");
  frmText.append(m_glyphsDb->getSurahNameGlyph(surah));

  // draw on top of the image the surah name text
  QPainter p(&baseImage);
  p.setPen(QPen(Qt::black));
  p.setFont(QFont("QCF_BSML", 77));
  p.drawText(baseImage.rect(), Qt::AlignCenter, frmText);

  if (Settings::darkMode)
    baseImage.invertPixels();

  return baseImage;
}

int
QuranPageBrowser::setHref(QTextCursor* cursor, int to, QString url)
{
  QTextCharFormat anchorFormat;
  anchorFormat.setAnchor(true);
  anchorFormat.setAnchorHref(url);

  int lastInsertPos = cursor->position();
  cursor->setPosition(to, QTextCursor::KeepAnchor);
  cursor->mergeCharFormat(anchorFormat);
  cursor->setPosition(lastInsertPos);

  return lastInsertPos;
}

QString
QuranPageBrowser::pageHeader(int page)
{
  m_headerData = m_quranDb->pageMetadata(page);

  QString suraHeader, jozzHeader;
  suraHeader.append("سورة ");
  suraHeader.append(m_quranDb->surahName(m_headerData.first, true));
  suraHeader.append("$");
  jozzHeader.append("الجزء ");
  jozzHeader.append(m_glyphsDb->getJuzGlyph(m_headerData.second));

  return suraHeader + jozzHeader;
}

void
QuranPageBrowser::constructPage(int pageNo, bool forceCustomSize)
{
  if (pageNo != m_page) {
    m_page = pageNo;
    m_highlightedIdx = -1;
  }
  // cleanup
  if (!m_verseCoordinates.empty())
    m_verseCoordinates.clear();
  this->document()->clear();

  m_pageFont = FontManager::pageFontname(pageNo);
  QTextCursor textCursor(this->document());

  m_currPageHeader = this->pageHeader(m_page);
  m_currPageLines = m_glyphsDb->getPageLines(m_page);

  // automatic font adjustment check
  if (!forceCustomSize && m_settings->value("Reader/AdaptiveFont").toBool()) {
    m_fontSize = this->bestFitFontSize();
    m_settings->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size",
                         m_fontSize);
  }

  m_pageLineSize = this->calcPageLineSize(m_currPageLines);

  // insert header in pages 3-604
  if (pageNo > 2) {
    m_headerTextFormat.setForeground(
      QBrush(qApp->palette().color(QPalette::PlaceholderText)));

    // smaller header font size for long juz > 10
    if (m_qcfVer == 1 && pageNo >= 202)
      m_headerTextFormat.setFontPointSize(std::max(4, m_fontSize - 8));
    else
      m_headerTextFormat.setFontPointSize(m_fontSize - 6);

    textCursor.insertBlock(m_pageFormat, m_headerTextFormat);
    textCursor.insertText(this->justifyHeader(m_currPageHeader));
    setHref(&textCursor, 1, "#F" + QString::number(m_headerData.first));
  }

  parentWidget()->setMinimumWidth(m_pageLineSize.width() + 70);

  // page lines drawing
  int counter = 0, prevAnchor = pageNo < 3 ? 0 : m_currPageHeader.size() + 1;
  m_bodyTextFormat.setFont(QFont(m_pageFont, m_fontSize));
  foreach (QString l, m_currPageLines) {
    l = l.trimmed();
    if (l.isEmpty())
      continue;

    if (l.contains("frame")) {
      // generate frame for surah
      int surah = l.split('_').at(1).toInt();
      QImage surahFrame = this->surahFrame(surah);
      // insert the surah image in the document
      textCursor.insertBlock(m_pageFormat, m_bodyTextFormat);
      textCursor.insertImage(surahFrame.scaledToWidth(
        m_pageLineSize.width() + 5, Qt::SmoothTransformation));

      setHref(&textCursor, prevAnchor, "#F" + QString::number(surah));
      prevAnchor += 2;
    } else if (l.contains("bsml")) {
      QImage bsml(":/resources/basmalah.png");
      if (Settings::darkMode)
        bsml.invertPixels();

      textCursor.insertBlock(m_pageFormat, m_bodyTextFormat);
      textCursor.insertImage(
        bsml.scaledToWidth(m_pageLineSize.width(), Qt::SmoothTransformation));
      prevAnchor += 2;
    } else {
      // pageline inertion operation
      textCursor.insertBlock(m_pageFormat, m_bodyTextFormat);
      // if contains verse separator character, add anchors
      if (l.contains(':')) {
        foreach (QChar glyph, l) {
          if (glyph != ':') {
            textCursor.insertText(glyph);
          } else {
            int lastInsertPos =
              setHref(&textCursor, prevAnchor, "#" + QString::number(counter));

            QPair<int, int> coords(prevAnchor, lastInsertPos);
            m_verseCoordinates.append(coords);

            counter++;
            prevAnchor = lastInsertPos;
          }
        }

      } else
        textCursor.insertText(l);
    }
  }

  // insert footer (page number)
  m_headerTextFormat.setForeground(qApp->palette().text());
  m_headerTextFormat.setFontPointSize(m_fontSize - 4);
  textCursor.insertBlock(m_pageFormat, m_headerTextFormat);
  textCursor.insertText(getEasternNum(QString::number(pageNo)));
  setAlignment(Qt::AlignCenter);
}

void
QuranPageBrowser::highlightVerse(int verseIdxInPage)
{
  if (verseIdxInPage > m_verseCoordinates.size() || verseIdxInPage < 0) {
    qCritical() << "verseIdxInPage is out of page coords range!!!";
    return;
  }

  resetHighlight();

  QTextCharFormat tcf;
  if (m_fgHighlight)
    tcf.setForeground(m_highlightColor);
  else
    tcf.setBackground(m_highlightColor);

  const QPair<int, int>& bounds = m_verseCoordinates.at(verseIdxInPage);

  m_highlighter->setPosition(bounds.first);
  m_highlighter->setPosition(bounds.second, QTextCursor::KeepAnchor);
  m_highlighter->mergeCharFormat(tcf);

  m_highlightedIdx = verseIdxInPage;
}

void
QuranPageBrowser::resetHighlight()
{
  QTextCharFormat tcf;
  if (m_fgHighlight)
    tcf.setForeground(Settings::darkMode ? Qt::white : Qt::black);
  else
    tcf.setBackground(Qt::transparent);

  if (m_highlighter->hasSelection())
    m_highlighter->mergeCharFormat(tcf); // de-highlight any previous highlights

  m_highlightedIdx = -1;
}

QuranPageBrowser::Action
QuranPageBrowser::lmbVerseMenu(bool favoriteVerse)
{
  QMenu lmbMenu(this);
  lmbMenu.addAction(m_actPlay);
  lmbMenu.addAction(m_actSelect);
  lmbMenu.addAction(m_actTafsir);
  lmbMenu.addAction(m_actTranslation);
  lmbMenu.addAction(m_actThoughts);
  lmbMenu.addSeparator();
  lmbMenu.addAction(m_actCopy);
  if (favoriteVerse) {
    lmbMenu.addAction(m_actRemBookmark);
  } else {
    lmbMenu.addAction(m_actAddBookmark);
  }

  QAction* chosen = lmbMenu.exec(QCursor::pos());

  QuranPageBrowser::Action actionIdx = Action::Null;
  if (chosen == m_actPlay)
    actionIdx = Action::Play;
  else if (chosen == m_actSelect)
    actionIdx = Action::Select;
  else if (chosen == m_actTafsir)
    actionIdx = Action::Tafsir;
  else if (chosen == m_actTranslation)
    actionIdx = Action::Translation;
  else if (chosen == m_actThoughts)
    actionIdx = Action::Thoughts;
  else if (chosen == m_actCopy)
    actionIdx = Action::Copy;
  else if (chosen == m_actAddBookmark)
    actionIdx = Action::AddBookmark;
  else if (chosen == m_actRemBookmark)
    actionIdx = Action::RemoveBookmark;

  this->clearFocus();
  return actionIdx;
}

int
QuranPageBrowser::bestFitFontSize()
{
  int sz;
  int margin = 50;
  static QRegularExpression rem("frame.*|bsml");
  for (sz = 28; sz >= 12; sz--) {
    QFont pf(m_pageFont, sz);
    QFontMetrics fm(pf);
    QFontMetrics headerMetrics(QFont("PakType Naskh Basic", sz - 6));
    QString completePage = m_currPageLines.join('\n');
    completePage.remove(rem);

    QSize textSz = headerMetrics.size(Qt::TextSingleLine, m_currPageHeader) +
                   fm.size(0, completePage);
    if (textSz.height() + margin <= parentWidget()->height()) {
      break;
    }
  }

  return sz;
}

void
QuranPageBrowser::createActions()
{
  m_actZoomIn = new QAction(tr("Zoom In"), this);
  m_actZoomOut = new QAction(tr("Zoom Out"), this);
  m_actCopy = new QAction(tr("Copy Verse"), this);
  m_actSelect = new QAction(tr("Select"), this);
  m_actPlay = new QAction(tr("Play"), this);
  m_actTafsir = new QAction(tr("Tafsir"), this);
  m_actTranslation = new QAction(tr("Translation"), this);
  m_actThoughts = new QAction(tr("Thoughts"), this);
  m_actAddBookmark = new QAction(tr("Add Bookmark"), this);
  m_actRemBookmark = new QAction(tr("Remove Bookmark"), this);
  m_actZoomIn->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_magnifying_glass_plus));
  m_actZoomOut->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_magnifying_glass_minus));
  m_actPlay->setIcon(StyleManager::awesome->icon(fa_solid, fa_play));
  m_actSelect->setIcon(StyleManager::awesome->icon(fa_solid, fa_hand_pointer));
  m_actTafsir->setIcon(StyleManager::awesome->icon(fa_solid, fa_book_open));
  m_actTranslation->setIcon(StyleManager::awesome->icon(fa_solid, fa_language));
  m_actThoughts->setIcon(StyleManager::awesome->icon(fa_solid, fa_comment));
  m_actCopy->setIcon(StyleManager::awesome->icon(fa_solid, fa_clipboard));
  m_actAddBookmark->setIcon(
    StyleManager::awesome->icon(fa_regular, fa_bookmark));
  m_actRemBookmark->setIcon(StyleManager::awesome->icon(fa_solid, fa_bookmark));
  connect(
    m_actZoomIn, &QAction::triggered, this, &QuranPageBrowser::actionZoomIn);
  connect(
    m_actZoomOut, &QAction::triggered, this, &QuranPageBrowser::actionZoomOut);
}

#ifndef QT_NO_CONTEXTMENU
void
QuranPageBrowser::contextMenuEvent(QContextMenuEvent* event)
{
  QMenu menu(this);
  menu.addAction(m_actZoomIn);
  menu.addAction(m_actZoomOut);

  m_mousePos = event->pos();
  m_mouseGlobalPos = event->globalPos();
  menu.exec(m_mouseGlobalPos);
}
#endif // QT_NO_CONTEXTMENU

void
QuranPageBrowser::actionZoomIn()
{
  m_fontSize++;
  m_settings->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size",
                       m_fontSize);
  constructPage(m_page, true);
  highlightVerse(m_highlightedIdx);
}

void
QuranPageBrowser::actionZoomOut()
{
  m_fontSize--;
  m_settings->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size",
                       m_fontSize);
  constructPage(m_page, true);
  highlightVerse(m_highlightedIdx);
}

void
QuranPageBrowser::updateHighlightLayer()
{
  int old = m_highlightedIdx;
  resetHighlight();
  m_fgHighlight = m_settings->value("Reader/FGHighlight").toBool();

  QColor hc = m_highlightColor.color();
  if (m_fgHighlight)
    hc.setAlpha(255);
  else
    hc.setAlpha(80);

  m_highlightColor.setColor(hc);
  if (old != -1)
    highlightVerse(old);
}

int
QuranPageBrowser::fontSize() const
{
  return m_fontSize;
}

QString
QuranPageBrowser::pageFont() const
{
  return m_pageFont;
}

int
QuranPageBrowser::page() const
{
  return m_page;
}
