#include "quranreader.h"
#include "../utils/fontmanager.h"
#include "../utils/shortcuthandler.h"
#include "../utils/stylemanager.h"
#include "../widgets/clickablelabel.h"
#include "ui_quranreader.h"
#include <QtAwesome.h>
using namespace fa;

QuranReader::QuranReader(QWidget* parent, VersePlayer* player)
  : QWidget(parent)
  , ui(new Ui::QuranReader)
  , m_player(player)
{
  ui->setupUi(this);
  setLayoutDirection(Qt::LeftToRight);
  loadIcons();
  loadReader();
  updateHighlight();
  updateSideFont();
  updateVerseType();
  redrawQuranPage(true);
  if (m_readerMode == ReaderMode::SinglePage)
    addSideContent();

  setupConnections();
}

void
QuranReader::loadIcons()
{
  ui->btnNext->setIcon(StyleManager::awesome->icon(fa_solid, fa_arrow_left));
  ui->btnPrev->setIcon(StyleManager::awesome->icon(fa_solid, fa_arrow_right));
}

void
QuranReader::loadReader()
{
  if (m_readerMode == ReaderMode::SinglePage) {
    m_activeQuranBrowser = m_quranBrowsers[0] =
      new QuranPageBrowser(ui->frmPageContent, m_currVerse->page());

    QWidget* scrollWidget = new QWidget();
    scrollWidget->setObjectName("scrollWidget");
    QVBoxLayout* vbl = new QVBoxLayout();
    vbl->setDirection(QBoxLayout::BottomToTop);
    scrollWidget->setLayout(vbl);

    m_scrlVerseByVerse = new QScrollArea;
    m_scrlVerseByVerse->setWidget(scrollWidget);
    m_scrlVerseByVerse->setWidgetResizable(true);
    m_scrlVerseByVerse->setStyleSheet(
      "QLabel, QAbstractScrollArea, QWidget#scrollWidget { "
      "background: "
      "transparent }");

    QHBoxLayout* lyt = qobject_cast<QHBoxLayout*>(ui->frmReader->layout());
    ui->frmSidePanel->layout()->addWidget(m_scrlVerseByVerse);
    lyt->setStretch(0, 1);
    lyt->setStretch(1, 0);
    this->setMinimumWidth(900);
  }

  else {
    // even Quran pages are always on the left side
    if (m_currVerse->page() % 2 == 0) {
      m_quranBrowsers[0] =
        new QuranPageBrowser(ui->frmPageContent, m_currVerse->page() - 1);
      m_activeQuranBrowser = m_quranBrowsers[1] =
        new QuranPageBrowser(ui->frmSidePanel, m_currVerse->page());

    } else {
      m_activeQuranBrowser = m_quranBrowsers[0] =
        new QuranPageBrowser(ui->frmPageContent, m_currVerse->page());
      m_quranBrowsers[1] =
        new QuranPageBrowser(ui->frmSidePanel, m_currVerse->page() + 1);
    }

    ui->frmSidePanel->layout()->addWidget(m_quranBrowsers[1]);
    QHBoxLayout* lyt = qobject_cast<QHBoxLayout*>(ui->frmReader->layout());
    lyt->insertSpacerItem(0, new QSpacerItem(20, 20, QSizePolicy::Expanding));
    lyt->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));

    lyt->setStretch(0, 1);
    lyt->setStretch(1, 0);
    lyt->setStretch(2, 0);
    lyt->setStretch(3, 1);
  }

  ui->frmPageContent->layout()->addWidget(m_quranBrowsers[0]);
}

void
QuranReader::setupConnections()
{
  connect(
    ui->btnNext, &QPushButton::clicked, this, &QuranReader::btnNextClicked);
  connect(
    ui->btnPrev, &QPushButton::clicked, this, &QuranReader::btnPrevClicked);
  for (int i = 0; i <= 1; i++)
    if (m_quranBrowsers[i])
      connect(m_quranBrowsers[i],
              &QTextBrowser::anchorClicked,
              this,
              &QuranReader::verseAnchorClicked);
  connect(this,
          &QuranReader::currentVerseChanged,
          m_player,
          &VersePlayer::loadActiveVerse);

  QSharedPointer<ShortcutHandler> handler = ShortcutHandler::current();
  connect(handler.data(),
          &ShortcutHandler::nextPage,
          this,
          &QuranReader::btnNextClicked);
  connect(handler.data(),
          &ShortcutHandler::prevPage,
          this,
          &QuranReader::btnPrevClicked);
  connect(handler.data(),
          &ShortcutHandler::toggleReaderView,
          this,
          &QuranReader::toggleReaderView);

  for (int i = 0; i <= 1; i++) {
    if (m_quranBrowsers[i]) {
      connect(handler.data(),
              &ShortcutHandler::zoomIn,
              m_quranBrowsers[i],
              &QuranPageBrowser::actionZoomIn);
      connect(handler.data(),
              &ShortcutHandler::zoomOut,
              m_quranBrowsers[i],
              &QuranPageBrowser::actionZoomOut);
    }
  }
}

void
QuranReader::toggleReaderView()
{
  if (ui->frmPageContent->isVisible() && ui->frmSidePanel->isVisible()) {
    ui->frmSidePanel->setVisible(false);
  } else if (ui->frmPageContent->isVisible()) {
    ui->frmSidePanel->setVisible(true);
    ui->frmPageContent->setVisible(false);
  } else
    ui->frmPageContent->setVisible(true);
}

void
QuranReader::updateSideFont()
{
  m_sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
}

void
QuranReader::updateVerseType()
{
  VerseType type =
    qvariant_cast<VerseType>(m_settings->value("Reader/VerseType"));
  m_versesFont.setFamily(FontManager::verseFontname(type, m_currVerse->page()));
  m_versesFont.setPointSize(m_settings->value("Reader/VerseFontSize").toInt());
  m_dbMgr->setVerseType(type);
}

void
QuranReader::updateHighlight()
{
  for (int i = 0; i <= 1; i++) {
    if (m_quranBrowsers[i]) {
      m_quranBrowsers[i]->updateHighlightLayer();
    }
  }
}

void
QuranReader::updatePageFontSize()
{
  for (int i = 0; i <= 1; i++)
    if (m_quranBrowsers[i])
      m_quranBrowsers[i]->updateFontSize();
}

void
QuranReader::redrawQuranPage(bool manualSz)
{
  if (m_activeQuranBrowser == m_quranBrowsers[0]) {
    m_quranBrowsers[0]->constructPage(m_currVerse->page(), manualSz);
    if (m_readerMode == Settings::DoublePage && m_quranBrowsers[1])
      m_quranBrowsers[1]->constructPage(m_currVerse->page() + 1, manualSz);
  } else {
    m_quranBrowsers[0]->constructPage(m_currVerse->page() - 1, manualSz);
    m_quranBrowsers[1]->constructPage(m_currVerse->page(), manualSz);
  }

  updatePageVerseInfoList();
}

void
QuranReader::addSideContent()
{
  if (m_readerMode != ReaderMode::SinglePage)
    return;

  if (!m_verseFrameList.isEmpty()) {
    qDeleteAll(m_verseFrameList);
    m_verseFrameList.clear();
    m_highlightedFrm = nullptr;
  }

  ClickableLabel* verselb;
  QLabel* contentLb;
  VerseFrame* verseContFrame;
  QString prevLbContent, currLbContent;
  if (m_dbMgr->getVerseType() == Settings::qcf)
    m_versesFont.setFamily(FontManager::pageFontname(m_currVerse->page()));

  for (int i = m_activeVList->size() - 1; i >= 0; i--) {
    const Verse* vInfo = &(m_activeVList->at(i));

    verseContFrame = new VerseFrame(m_scrlVerseByVerse->widget());
    verselb = new ClickableLabel(verseContFrame);
    contentLb = new QLabel(verseContFrame);

    verseContFrame->setObjectName(
      QString("%0_%1").arg(vInfo->surah()).arg(vInfo->number()));

    verselb->setFont(m_versesFont);
    verselb->setText(m_dbMgr->getVerseGlyphs(vInfo->surah(), vInfo->number()));
    verselb->setAlignment(Qt::AlignCenter);
    verselb->setWordWrap(true);

    currLbContent = m_dbMgr->getTranslation(vInfo->surah(), vInfo->number());

    if (currLbContent == prevLbContent) {
      currLbContent = '-';
    } else {
      prevLbContent = currLbContent;
    }

    contentLb->setText(currLbContent);
    contentLb->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contentLb->setAlignment(Qt::AlignCenter);
    contentLb->setWordWrap(true);
    contentLb->setFont(m_sideFont);

    verseContFrame->layout()->addWidget(verselb);
    verseContFrame->layout()->addWidget(contentLb);
    m_scrlVerseByVerse->widget()->layout()->addWidget(verseContFrame);
    m_verseFrameList.insert(0, verseContFrame);

    // connect clicked signal for each label
    connect(
      verselb, &ClickableLabel::clicked, this, &QuranReader::verseClicked);
  }

  if (m_player->playbackState() == QMediaPlayer::PlayingState) {
    setHighlightedFrame();
  }
}

void
QuranReader::highlightCurrentVerse()
{
  if (m_currVerse->number() == 0)
    return;

  // idx may be -1 if verse number is 0 (basmallah)
  int idx = m_activeVList->indexOf(*m_currVerse);
  if (idx < 0)
    idx = 0;

  m_activeQuranBrowser->highlightVerse(idx);

  if (m_readerMode == ReaderMode::SinglePage)
    setHighlightedFrame();
}

void
QuranReader::setHighlightedFrame()
{
  if (m_highlightedFrm != nullptr)
    m_highlightedFrm->setSelected(false);

  VerseFrame* verseFrame = m_scrlVerseByVerse->widget()->findChild<VerseFrame*>(
    QString("%0_%1").arg(QString::number(m_currVerse->surah()),
                         QString::number(m_currVerse->number())));

  verseFrame->setSelected(true);

  m_scrlVerseByVerse->ensureWidgetVisible(verseFrame);
  m_highlightedFrm = verseFrame;
}

void
QuranReader::navigateToVerse(const Verse& v)
{
  gotoPage(v.page(), false);

  m_currVerse->update(v);
  emit currentSurahChanged();
  emit currentVerseChanged();
  highlightCurrentVerse();
}

void
QuranReader::updatePageVerseInfoList()
{
  if (m_activeQuranBrowser == m_quranBrowsers[0]) {
    m_vLists[0] =
      Verse::fromList(m_dbMgr->getVerseInfoList(m_currVerse->page()));
    if (m_readerMode == Settings::DoublePage)
      m_vLists[1] =
        Verse::fromList(m_dbMgr->getVerseInfoList(m_currVerse->page() + 1));

    m_activeVList = &m_vLists[0];

  } else {
    m_vLists[0] =
      Verse::fromList(m_dbMgr->getVerseInfoList(m_currVerse->page() - 1));
    m_vLists[1] =
      Verse::fromList(m_dbMgr->getVerseInfoList(m_currVerse->page()));

    m_activeVList = &m_vLists[1];
  }
}

void
QuranReader::btnNextClicked()
{
  if (m_readerMode == ReaderMode::SinglePage || m_currVerse->page() % 2 == 0)
    nextPage(1);
  else
    nextPage(2);
}

void
QuranReader::btnPrevClicked()
{
  if (m_readerMode == ReaderMode::SinglePage || m_currVerse->page() % 2 != 0)
    prevPage(1);
  else
    prevPage(2);
}

bool
QuranReader::areNeighbors(int page1, int page2)
{
  return page1 % 2 != 0 && page2 % 2 == 0 && page2 == page1 + 1;
}

void
QuranReader::switchActivePage()
{
  if (!m_quranBrowsers[1])
    return;

  m_activeQuranBrowser->resetHighlight();
  if (m_activeQuranBrowser == m_quranBrowsers[0]) {
    m_activeQuranBrowser = m_quranBrowsers[1];
    m_activeVList = &m_vLists[1];
  } else {
    m_activeQuranBrowser = m_quranBrowsers[0];
    m_activeVList = &m_vLists[0];
  }
}

void
QuranReader::selectVerse(int browserIdx, int IdxInPage)
{
  if (m_activeQuranBrowser != m_quranBrowsers[browserIdx])
    switchActivePage();

  const Verse& v = m_vLists[browserIdx].at(IdxInPage);
  int currSurah = m_currVerse->surah();
  m_currVerse->update(v);
  emit currentVerseChanged();

  if (currSurah != v.surah())
    emit currentSurahChanged();
}

void
QuranReader::setVerseToStartOfPage()
{
  // set the current verse to the verse at the top of the page
  m_currVerse->update(m_activeVList->at(0));
  m_player->loadActiveVerse();
}

void
QuranReader::verseAnchorClicked(const QUrl& hrefUrl)
{
  if (hrefUrl.toString().at(1) == 'F') {
    int surah = hrefUrl.toString().remove("#F").toInt();
    qDebug() << "SURAH CARD:" << surah;
    emit showBetaqa(surah);
    return;
  }

  QuranPageBrowser* senderBrowser = qobject_cast<QuranPageBrowser*>(sender());
  int browerIdx = senderBrowser == m_quranBrowsers[1];
  int idx = hrefUrl.toString().remove('#').toInt();
  Verse v(m_vLists[browerIdx].at(idx));

  QuranPageBrowser::Action chosenAction =
    senderBrowser->lmbVerseMenu(m_dbMgr->isBookmarked(v.toList()));

  switch (chosenAction) {
    case QuranPageBrowser::play:
      selectVerse(browerIdx, idx);
      highlightCurrentVerse();
      m_player->playCurrentVerse();
      break;
    case QuranPageBrowser::select:
      selectVerse(browerIdx, idx);
      m_player->loadActiveVerse();
      highlightCurrentVerse();
      break;
    case QuranPageBrowser::tafsir:
      emit showVerseTafsir(v);
      break;
    case QuranPageBrowser::copy:
      emit copyVerseText(v);
      break;
    case QuranPageBrowser::addBookmark:
      m_dbMgr->addBookmark(v.toList());
      break;
    case QuranPageBrowser::removeBookmark:
      if (m_dbMgr->removeBookmark(v.toList()))
        break;
    default:
      break;
  }
}

void
QuranReader::verseClicked()
{
  // object = clickable label, parent = verse frame, verse frame name scheme =
  // 'surah_verse'
  QStringList data = sender()->parent()->objectName().split('_');
  int surah = data.at(0).toInt();
  int verse = data.at(1).toInt();

  m_currVerse->setNumber(verse);
  emit currentVerseChanged();

  if (m_currVerse->surah() != surah) {
    m_currVerse->setSurah(surah);
    emit currentSurahChanged();
  }

  m_player->loadActiveVerse();
  highlightCurrentVerse();
  m_player->play();
}

void
QuranReader::gotoPage(int page, bool updateElements, bool automaticFlip)
{
  m_activeQuranBrowser->resetHighlight();

  if (!automaticFlip)
    m_player->stop();

  if (m_activeQuranBrowser->page() != page) {
    if (m_readerMode == ReaderMode::SinglePage)
      gotoSinglePage(page);
    else
      gotoDoublePage(page);
  }

  if (updateElements) {
    setVerseToStartOfPage();
    emit currentVerseChanged();
    emit currentSurahChanged();
  }
}

void
QuranReader::gotoSinglePage(int page)
{
  m_currVerse->setPage(page);
  redrawQuranPage();

  m_currVerse->update(m_activeVList->at(0));
  addSideContent();
}

void
QuranReader::gotoDoublePage(int page)
{
  int currPage = m_currVerse->page();
  m_currVerse->setPage(page);

  int pageBrowerIdx = page % 2 == 0;

  if (areNeighbors(currPage, page) || areNeighbors(page, currPage))
    switchActivePage();
  else {
    m_activeQuranBrowser = m_quranBrowsers[pageBrowerIdx];
    redrawQuranPage();
  }
}

void
QuranReader::nextPage(int step)
{
  bool keepPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
  if (m_currVerse->page() + step <= 604) {
    gotoPage(m_currVerse->page() + step, true, true);

    if (m_readerMode == ReaderMode::SinglePage)
      m_scrlVerseByVerse->verticalScrollBar()->setValue(0);

    // if the page is flipped automatically, resume playback
    if (keepPlaying) {
      m_player->play();
      highlightCurrentVerse();
    }
  }
}

void
QuranReader::prevPage(int step)
{
  bool keepPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
  if (m_currVerse->page() - step >= 1) {
    gotoPage(m_currVerse->page() - step, true, true);

    if (m_readerMode == ReaderMode::SinglePage)
      m_scrlVerseByVerse->verticalScrollBar()->setValue(0);

    if (keepPlaying) {
      m_player->play();
      highlightCurrentVerse();
    }
  }
}

void
QuranReader::gotoSurah(int surahIdx)
{
  // getting surah index
  int page = m_dbMgr->getSurahStartPage(surahIdx);
  gotoPage(page, false);

  m_currVerse->setPage(page);
  m_currVerse->setSurah(surahIdx);
  m_currVerse->setNumber((surahIdx == 9 || surahIdx == 1) ? 1 : 0);

  // syncing the player & playing basmalah
  m_player->playCurrentVerse();

  highlightCurrentVerse();
  emit currentVerseChanged();
  emit currentSurahChanged();
}

QuranReader::~QuranReader()
{
  delete ui;
}
