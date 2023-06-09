#include "quranpagebrowser.h"

QuranPageBrowser::QuranPageBrowser(QWidget* parent,
                                   int qcfVersion,
                                   int initPage,
                                   DBManager* dbPtr,
                                   QSettings* appSettings,
                                   const QString& iconsPath)
  : QTextBrowser(parent)
  , m_resourcePath{ iconsPath }
  , m_dbMgr{ dbPtr }
  , m_qcfVer{ qcfVersion }
  , m_settingsPtr{ appSettings }
  , m_highlighter{ new QTextCursor(document()) }
  , m_highlightColor{ QBrush(QColor(0, 161, 185)) }
  , m_darkMode{ appSettings->value("Theme").toInt() == 1 }
  , m_bsmlFont{ qcfVersion == 1 ? "QCF_BSML" : "QCF2BSML" }
  , m_fontPrefix{ qcfVersion == 1 ? "QCF_P" : "QCF2" }

{

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  verticalScrollBar()->setVisible(false);
  setStyleSheet("QTextBrowser{background-color: transparent;}");
  setTextInteractionFlags(Qt::TextInteractionFlag::LinksAccessibleByMouse);
  createActions();
  updateFontSize();

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

QString
QuranPageBrowser::getEasternNum(QString num)
{
  QString easternNum;

  for (int i = 0; i < num.size(); i++) {
    easternNum.append(m_easternNumsMap.value(num[i]));
  }
  return easternNum;
}

QString
QuranPageBrowser::constructPageHeader(int page)
{
  QList<int> headerData = m_dbMgr->getPageMetadata(page);

  QString suraHeader, jozzHeader;
  suraHeader.append("ﮌ");
  suraHeader.append(m_dbMgr->getSurahNameGlyph(headerData.at(0)));
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
  jozzHeader.append("ﰦ");
  jozzHeader.append("ﰦ");
  jozzHeader.append("ﰸ");
  jozzHeader.append(m_dbMgr->getJuzGlyph(headerData.at(1)));

  return suraHeader + jozzHeader;
}

void
QuranPageBrowser::constructPage(int pageNo, bool manualSz)
{
  if (pageNo != m_page)
    m_highlightedIdx = -1;

  if (!m_pageVerseCoords.empty()) {
    qDeleteAll(m_pageVerseCoords);
    m_pageVerseCoords.clear();
  }

  this->document()->clear();
  QTextCursor cur(this->document());

  m_page = pageNo;
  m_pageFont = m_fontPrefix;

  m_pageFont.append(QString::number(pageNo).rightJustified(3, '0'));

  QString header = constructPageHeader(pageNo);
  QStringList lines =
    m_dbMgr->getPageLines(pageNo); // create a qlist of page lines

  int fontSize;
  manualSz = manualSz || !m_settingsPtr->value("Reader/AdaptiveFont").toBool();
  if (manualSz)
    fontSize = pageNo < 3 ? m_fontSize + 5 : m_fontSize;
  else {
    m_fontSize = fontSize = bestFitFontSize(lines);
    m_settingsPtr->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size",
                            fontSize);
  }

  int counter = 0, prevAnchor = pageNo < 3 ? 3 : 28;
  QTextBlockFormat pageFormat;
  pageFormat.setAlignment(Qt::AlignCenter);
  pageFormat.setNonBreakableLines(true);
  pageFormat.setLayoutDirection(Qt::RightToLeft);

  QTextCharFormat bsmlFormat, pageTextFormat;
  bsmlFormat.setFont(QFont(m_bsmlFont, fontSize - 6));
  pageTextFormat.setFont(QFont(m_pageFont, fontSize));
  if (pageNo > 2) {
    cur.insertBlock(pageFormat, bsmlFormat);
    cur.insertText(header);
    bsmlFormat.setFontPointSize(fontSize - 2);
  }

  QFontMetrics fm(QFont(m_pageFont, fontSize));
  QString measureLine;
  if (pageNo < 3) {
    measureLine = lines.at(3);
  } else if (pageNo >= 602 || pageNo == 596) {
    measureLine = lines.at(2);
  } else {
    measureLine = lines.at(lines.size() - 2);
  }

  m_pageLineSize =
    fm.size(Qt::TextSingleLine, measureLine.remove(':')) + QSize(0, 5);
  this->parentWidget()->setMinimumWidth(m_pageLineSize.width() + 50);
  this->setMinimumWidth(m_pageLineSize.width() + 50);

  foreach (QString l, lines) {
    l = l.trimmed();
    if (l.isEmpty())
      continue;

    if (l.contains("frame")) {
      QImage frm(":/resources/sura_box.png"); // load the empty frame

      // construct the text to be put inside the frame
      QString frmText;
      frmText.append("ﰦ");
      frmText.append("ﮌ");
      frmText.append(m_dbMgr->getSurahNameGlyph(l.split('_').at(1).toInt()));

      // draw on top of the image the surah name text
      QPainter p(&frm);
      p.setPen(QPen(Qt::black));
      p.setFont(QFont(m_bsmlFont, 77));
      p.drawText(frm.rect(), Qt::AlignCenter, frmText);

      if (m_darkMode)
        frm.invertPixels();

      // insert the surah image in the document
      cur.insertBlock(pageFormat, pageTextFormat);
      cur.insertImage(
        frm.scaledToWidth(m_pageLineSize.width(), Qt::SmoothTransformation));

    } else if (l.contains("bsml")) {
      QImage bsml(":/resources/basmalah.png");
      if (m_darkMode)
        bsml.invertPixels();

      cur.insertBlock(pageFormat, pageTextFormat);
      cur.insertImage(
        bsml.scaledToWidth(m_pageLineSize.width(), Qt::SmoothTransformation));

    } else {
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

            int* coords = new int[2];
            coords[0] = prevAnchor;
            coords[1] = lastInsertPos;
            m_pageVerseCoords.append(coords);

            counter++;
            prevAnchor = lastInsertPos;
            cur.setPosition(lastInsertPos);
          }
        }

      } else
        cur.insertText(l);
    }
  }

  pageTextFormat.setFont(QFont("PakType Naskh Basic", m_fontSize - 4));
  cur.insertBlock(pageFormat, pageTextFormat);
  cur.insertText(getEasternNum(QString::number(pageNo)));
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
QuranPageBrowser::bestFitFontSize(QStringList& lines)
{
  int sz;
  int margin = m_qcfVer == 1 ? 50 : 25;
  for (sz = 28; sz >= 12; sz--) {
    QFont pf(m_pageFont, sz);
    QFontMetrics fm(pf);

    QSize textSz = fm.size(0, lines.join('\n')) + m_pageLineSize;
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
  m_zoomIn->setIcon(QIcon(m_resourcePath + "/icons/zoom-in.png"));
  m_zoomOut->setIcon(QIcon(m_resourcePath + "/icons/zoom-out.png"));
  m_playAct->setIcon(QIcon(m_resourcePath + "/icons/play.png"));
  m_selectAct->setIcon(QIcon(m_resourcePath + "/icons/select.png"));
  m_tafsirAct->setIcon(QIcon(m_resourcePath + "/icons/tafsir.png"));
  m_copyAct->setIcon(QIcon(m_resourcePath + "/icons/copy.png"));
  m_actAddBookmark->setIcon(
    QIcon(m_resourcePath + "/icons/bookmark-false.png"));
  m_actRemBookmark->setIcon(QIcon(m_resourcePath + "/icons/bookmark-true.png"));
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
  m_settingsPtr->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size",
                          m_fontSize);
  constructPage(m_page, true);
  highlightVerse(m_highlightedIdx);
}

void
QuranPageBrowser::actionZoomOut()
{
  m_fontSize--;
  m_settingsPtr->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size",
                          m_fontSize);
  constructPage(m_page, true);
  highlightVerse(m_highlightedIdx);
}

void
QuranPageBrowser::actionCopy()
{
  int posInDoc = cursorForPosition(m_mousePos).position();

  int idxInPage;
  for (idxInPage = 0; idxInPage < m_pageVerseCoords.size(); ++idxInPage) {
    const int* const vCoord = m_pageVerseCoords.at(idxInPage);
    if (vCoord[0] <= posInDoc && vCoord[1] >= posInDoc)
      break;
  }

  emit copyVerse(idxInPage);
}

void
QuranPageBrowser::updateFontSize()
{
  m_fontSize =
    m_settingsPtr->value("Reader/QCF" + QString::number(m_qcfVer) + "Size", 22)
      .toInt();
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
