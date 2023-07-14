#include "quranpagebrowser.h"

QuranPageBrowser::QuranPageBrowser(QWidget* parent,
                                   DBManager* dbMgr,
                                   int initPage)
  : QTextBrowser(parent)
  , m_dbMgr{ dbMgr }
  , m_highlighter{ new QTextCursor(document()) }
  , m_highlightColor{ QBrush(QColor(0, 161, 185)) }
{

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  verticalScrollBar()->setVisible(false);
  setStyleSheet("QTextBrowser{background-color: transparent;}");
  setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByMouse);
  createActions();
  updateFontSize();

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

  QShortcut* zoomIn = new QShortcut(QKeySequence::StandardKey::ZoomIn, this);
  QShortcut* zoomOut = new QShortcut(QKeySequence::StandardKey::ZoomOut, this);
  connect(zoomIn, &QShortcut::activated, this, &QuranPageBrowser::actionZoomIn);
  connect(
    zoomOut, &QShortcut::activated, this, &QuranPageBrowser::actionZoomOut);
}

void
QuranPageBrowser::updateFontSize()
{
  m_fontSize =
    m_settings->value("Reader/QCF" + QString::number(m_qcfVer) + "Size", 22)
      .toInt();
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

/*!
 * \brief QuranPageBrowser::justifyHeader adjusts the header string according to
 * the page width
 * \param baseHeader header string
 * \return reference to the adjusted string
 */
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
  frmText.append(m_dbMgr->getSurahNameGlyph(surah));

  // draw on top of the image the surah name text
  QPainter p(&baseImage);
  p.setPen(QPen(Qt::black));
  p.setFont(QFont(m_bsmlFont, 77));
  p.drawText(baseImage.rect(), Qt::AlignCenter, frmText);

  if (m_darkMode)
    baseImage.invertPixels();

  return baseImage;
}

QString
QuranPageBrowser::pageHeader(int page)
{
  QList<int> headerData = m_dbMgr->getPageMetadata(page);

  QString suraHeader, jozzHeader;
  suraHeader.append("سورة ");
  suraHeader.append(m_dbMgr->getSurahName(headerData.at(0), true));
  suraHeader.append("$");
  jozzHeader.append("الجزء ");
  jozzHeader.append(m_dbMgr->getJuzGlyph(headerData.at(1)));

  return suraHeader + jozzHeader;
}

void
QuranPageBrowser::constructPage(int pageNo, bool manualSz)
{
  if (pageNo != m_page) {
    m_page = pageNo;
    m_highlightedIdx = -1;
  }
  // cleanup
  if (!m_pageVerseCoords.empty()) {
    qDeleteAll(m_pageVerseCoords);
    m_pageVerseCoords.clear();
  }
  this->document()->clear();

  m_pageFont = m_fontnamePrefix;
  m_pageFont.append(QString::number(m_page).rightJustified(3, '0'));
  QTextCursor textCursor(this->document());

  m_currPageHeader = this->pageHeader(m_page);
  m_currPageLines = m_dbMgr->getPageLines(m_page);

  // automatic font adjustment check
  if (manualSz || !m_settings->value("Reader/AdaptiveFont").toBool())
    m_fontSize = pageNo < 3 ? m_fontSize + 5 : m_fontSize;
  else {
    m_fontSize = this->bestFitFontSize();
    m_settings->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size",
                         m_fontSize);
  }

  m_pageLineSize = this->calcPageLineSize(m_currPageLines);

  // insert header in pages 3-604
  if (pageNo > 2) {
    m_headerTextFormat.setForeground(QBrush(Qt::gray));
    m_headerTextFormat.setFontPointSize(m_fontSize - 6);
    textCursor.insertBlock(m_pageFormat, m_headerTextFormat);
    textCursor.insertText(this->justifyHeader(m_currPageHeader));
  }

  this->parentWidget()->setMinimumWidth(m_pageLineSize.width() + 70);
  this->setMinimumWidth(m_pageLineSize.width() + 70);

  // page lines drawing
  int counter = 0, prevAnchor = pageNo < 3 ? 3 : m_currPageHeader.size() + 2;
  m_bodyTextFormat.setFont(QFont(m_pageFont, m_fontSize));
  foreach (QString l, m_currPageLines) {
    l = l.trimmed();
    if (l.isEmpty())
      continue;

    if (l.contains("frame")) {
      // generate frame for surah
      QImage surahFrame = this->surahFrame(l.split('_').at(1).toInt());
      // insert the surah image in the document
      textCursor.insertBlock(m_pageFormat, m_bodyTextFormat);
      textCursor.insertImage(surahFrame.scaledToWidth(
        m_pageLineSize.width() + 5, Qt::SmoothTransformation));
    } else if (l.contains("bsml")) {
      QImage bsml(":/resources/basmalah.png");
      if (m_darkMode)
        bsml.invertPixels();

      textCursor.insertBlock(m_pageFormat, m_bodyTextFormat);
      textCursor.insertImage(
        bsml.scaledToWidth(m_pageLineSize.width(), Qt::SmoothTransformation));
    } else {
      // pageline inertion operation
      textCursor.insertBlock(m_pageFormat, m_bodyTextFormat);
      // if contains verse separator character, add anchors
      if (l.contains(':')) {
        foreach (QChar glyph, l) {
          if (glyph != ':') {
            textCursor.insertText(glyph);
          } else {
            QTextCharFormat anchorFormat;
            anchorFormat.setAnchor(true);
            anchorFormat.setAnchorHref("#" + QString::number(counter));

            int lastInsertPos = textCursor.position();
            textCursor.setPosition(prevAnchor, QTextCursor::KeepAnchor);
            textCursor.mergeCharFormat(anchorFormat);

            int* coords = new int[2];
            coords[0] = prevAnchor;
            coords[1] = lastInsertPos;
            m_pageVerseCoords.append(coords);

            counter++;
            prevAnchor = lastInsertPos;
            textCursor.setPosition(lastInsertPos);
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
  if (verseIdxInPage > m_pageVerseCoords.size() || verseIdxInPage < 0) {
    qCritical() << "verseIdxInPage is out of page coords range!!!";
    return;
  }

  QTextCharFormat tcf;
  tcf.setForeground(m_darkMode ? Qt::white : Qt::black);

  if (m_highlighter->hasSelection())
    m_highlighter->mergeCharFormat(tcf); // de-highlight any previous highlights

  tcf.setForeground(m_highlightColor);

  const int* const bounds = m_pageVerseCoords.at(verseIdxInPage);

  m_highlighter->setPosition(bounds[0]);
  m_highlighter->setPosition(bounds[1], QTextCursor::KeepAnchor);
  m_highlighter->mergeCharFormat(tcf);

  qInfo() << "Selected verse #" + QString::number(verseIdxInPage) + " in page";

  m_highlightedIdx = verseIdxInPage;
}

int
QuranPageBrowser::lmbVerseMenu(bool favoriteVerse)
{
  QMenu lmbMenu(this);
  lmbMenu.addAction(m_playAct);
  lmbMenu.addAction(m_selectAct);
  lmbMenu.addAction(m_tafsirAct);
  lmbMenu.addSeparator();
  lmbMenu.addAction(m_copyAct);
  if (favoriteVerse) {
    lmbMenu.addAction(m_actRemBookmark);
  } else {
    lmbMenu.addAction(m_actAddBookmark);
  }

  QAction* chosen = lmbMenu.exec(QCursor::pos());

  int actionIdx = -1;
  if (chosen == m_playAct)
    actionIdx = 0;
  else if (chosen == m_selectAct)
    actionIdx = 1;
  else if (chosen == m_tafsirAct)
    actionIdx = 2;
  else if (chosen == m_copyAct)
    actionIdx = 3;
  else if (chosen == m_actAddBookmark)
    actionIdx = 4;
  else if (chosen == m_actRemBookmark)
    actionIdx = 5;

  this->clearFocus();
  return actionIdx;
}

int
QuranPageBrowser::bestFitFontSize()
{
  int sz;
  int margin = 50;
  for (sz = 28; sz >= 12; sz--) {
    QFont pf(m_pageFont, sz);
    QFontMetrics fm(pf);
    QFontMetrics headerMetrics(QFont("PakType Naskh Basic", sz - 6));

    QSize textSz = headerMetrics.size(Qt::TextSingleLine, m_currPageHeader) +
                   fm.size(0, m_currPageLines.join('\n'));
    if (textSz.height() + margin <= parentWidget()->height()) {
      break;
    }
  }

  return sz;
}

void
QuranPageBrowser::createActions()
{
  m_zoomIn = new QAction(tr("Zoom In"), this);
  m_zoomOut = new QAction(tr("Zoom Out"), this);
  m_copyAct = new QAction(tr("Copy Verse"), this);
  m_selectAct = new QAction(tr("Select"), this);
  m_playAct = new QAction(tr("Play"), this);
  m_tafsirAct = new QAction(tr("Tafsir"), this);
  m_actAddBookmark = new QAction(tr("Add Bookmark"), this);
  m_actRemBookmark = new QAction(tr("Remove Bookmark"), this);
  m_zoomIn->setIcon(QIcon(m_resources.filePath("icons/zoom-in.png")));
  m_zoomOut->setIcon(QIcon(m_resources.filePath("icons/zoom-out.png")));
  m_playAct->setIcon(QIcon(m_resources.filePath("icons/play.png")));
  m_selectAct->setIcon(QIcon(m_resources.filePath("icons/select.png")));
  m_tafsirAct->setIcon(QIcon(m_resources.filePath("icons/tafsir.png")));
  m_copyAct->setIcon(QIcon(m_resources.filePath("icons/copy.png")));
  m_actAddBookmark->setIcon(
    QIcon(m_resources.filePath("icons/bookmark-false.png")));
  m_actRemBookmark->setIcon(
    QIcon(m_resources.filePath("icons/bookmark-true.png")));
  connect(m_zoomIn, &QAction::triggered, this, &QuranPageBrowser::actionZoomIn);
  connect(
    m_zoomOut, &QAction::triggered, this, &QuranPageBrowser::actionZoomOut);
}

#ifndef QT_NO_CONTEXTMENU
void
QuranPageBrowser::contextMenuEvent(QContextMenuEvent* event)
{
  QMenu menu(this);
  menu.addAction(m_zoomIn);
  menu.addAction(m_zoomOut);

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
