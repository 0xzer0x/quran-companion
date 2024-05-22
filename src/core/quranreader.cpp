#include "quranreader.h"
#include "ui_quranreader.h"
#include <QtAwesome.h>
#include <utils/fontmanager.h>
#include <utils/servicefactory.h>
#include <utils/shortcuthandler.h>
#include <utils/stylemanager.h>
#include <widgets/clickablelabel.h>
using namespace fa;

QuranReader::QuranReader(QWidget* parent, VersePlayer* player)
  : QWidget(parent)
  , ui(new Ui::QuranReader)
  , m_player(player)
  , m_currVerse(Verse::getCurrent())
  , m_config(Configuration::getInstance())
  , m_tafasir(Tafsir::tafasir)
  , m_tafsirService(ServiceFactory::tafsirService())
  , m_translationService(ServiceFactory::translationService())
  , m_bookmarkService(ServiceFactory::bookmarkService())
  , m_quranService(ServiceFactory::quranService())
  , m_glyphService(ServiceFactory::glyphService())
{
  ui->setupUi(this);
  setLayoutDirection(Qt::LeftToRight);
  loadIcons();
  loadReader();
  updateHighlight();
  updateSideFont();
  updateVerseType();
  redrawQuranPage(true);
  if (m_config.readerMode() == ReaderMode::SinglePage)
    addSideContent();

  setupConnections();
}

void
QuranReader::loadIcons()
{
  ui->btnNext->setIcon(
    StyleManager::getInstance().awesome().icon(fa_solid, fa_arrow_left));
  ui->btnPrev->setIcon(
    StyleManager::getInstance().awesome().icon(fa_solid, fa_arrow_right));
}

void
QuranReader::loadReader()
{
  if (m_config.readerMode() == ReaderMode::SinglePage) {
    m_activeQuranBrowser = m_quranBrowsers[0] =
      new QuranPageBrowser(ui->frmPageContent, m_currVerse.page());

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
    if (m_currVerse.page() % 2 == 0) {
      m_quranBrowsers[0] =
        new QuranPageBrowser(ui->frmPageContent, m_currVerse.page() - 1);
      m_activeQuranBrowser = m_quranBrowsers[1] =
        new QuranPageBrowser(ui->frmSidePanel, m_currVerse.page());

    } else {
      m_activeQuranBrowser = m_quranBrowsers[0] =
        new QuranPageBrowser(ui->frmPageContent, m_currVerse.page());
      m_quranBrowsers[1] =
        new QuranPageBrowser(ui->frmSidePanel, m_currVerse.page() + 1);
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

  const ShortcutHandler& handler = ShortcutHandler::getInstance();
  connect(
    &handler, &ShortcutHandler::nextPage, this, &QuranReader::btnNextClicked);
  connect(
    &handler, &ShortcutHandler::prevPage, this, &QuranReader::btnPrevClicked);
  connect(&handler,
          &ShortcutHandler::toggleReaderView,
          this,
          &QuranReader::toggleReaderView);

  for (int i = 0; i <= 1; i++) {
    if (m_quranBrowsers[i]) {
      connect(&handler,
              &ShortcutHandler::zoomIn,
              m_quranBrowsers[i],
              &QuranPageBrowser::actionZoomIn);
      connect(&handler,
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
    qvariant_cast<QFont>(m_config.settings().value("Reader/SideContentFont"));
}

void
QuranReader::updateVerseType()
{
  Configuration::VerseType type = qvariant_cast<Configuration::VerseType>(
    m_config.settings().value("Reader/VerseType"));
  m_versesFont.setFamily(
    FontManager::getInstance().verseFontname(type, m_currVerse.page()));
  m_versesFont.setPointSize(
    m_config.settings().value("Reader/VerseFontSize").toInt());
  m_config.setVerseType(type);
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
    m_quranBrowsers[0]->constructPage(m_currVerse.page(), manualSz);
    if (m_config.readerMode() == Configuration::DoublePage &&
        m_quranBrowsers[1])
      m_quranBrowsers[1]->constructPage(m_currVerse.page() + 1, manualSz);
  } else {
    m_quranBrowsers[0]->constructPage(m_currVerse.page() - 1, manualSz);
    m_quranBrowsers[1]->constructPage(m_currVerse.page(), manualSz);
  }

  updatePageVerseInfoList();
}

void
QuranReader::addSideContent()
{
  if (m_config.readerMode() != ReaderMode::SinglePage)
    return;

  if (!m_verseFrameList.isEmpty()) {
    qDeleteAll(m_verseFrameList);
    m_verseFrameList.clear();
    m_highlightedFrm = nullptr;
  }

  ClickableLabel* verselb;
  QLabel* contentLb;
  VerseFrame* verseContFrame;
  QString prevLbContent, currLbContent, glyphs;
  if (m_config.verseType() == Configuration::Qcf)
    m_versesFont.setFamily(
      FontManager::getInstance().pageFontname(m_currVerse.page()));

  m_translationService->setCurrentTranslation(
    m_config.settings().value("Reader/Translation").toInt());
  for (int i = m_activeVList->size() - 1; i >= 0; i--) {
    const Verse* verse = &(m_activeVList->at(i));

    verseContFrame = new VerseFrame(m_scrlVerseByVerse->widget());
    verselb = new ClickableLabel(verseContFrame);
    contentLb = new QLabel(verseContFrame);
    glyphs = m_config.verseType() == Configuration::Qcf
               ? m_glyphService->getVerseGlyphs(verse->surah(), verse->number())
               : m_quranService->verseText(verse->surah(), verse->number());

    verseContFrame->setObjectName(
      QString("%0_%1").arg(verse->surah()).arg(verse->number()));

    verselb->setFont(m_versesFont);
    verselb->setText(glyphs);
    verselb->setAlignment(Qt::AlignCenter);
    verselb->setWordWrap(true);

    currLbContent =
      m_translationService->getTranslation(verse->surah(), verse->number());

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
  if (m_currVerse.number() == 0)
    return;

  // idx may be -1 if verse number is 0 (basmallah)
  int idx = m_activeVList->indexOf(m_currVerse);
  if (idx < 0)
    idx = 0;

  m_activeQuranBrowser->highlightVerse(idx);

  if (m_config.readerMode() == ReaderMode::SinglePage)
    setHighlightedFrame();
}

void
QuranReader::setHighlightedFrame()
{
  if (m_highlightedFrm != nullptr)
    m_highlightedFrm->setSelected(false);

  VerseFrame* verseFrame = m_scrlVerseByVerse->widget()->findChild<VerseFrame*>(
    QString("%0_%1").arg(QString::number(m_currVerse.surah()),
                         QString::number(m_currVerse.number())));

  verseFrame->setSelected(true);

  m_scrlVerseByVerse->ensureWidgetVisible(verseFrame);
  m_highlightedFrm = verseFrame;
}

void
QuranReader::navigateToVerse(const Verse& v)
{
  gotoPage(v.page(), false);

  m_currVerse.update(v);
  emit currentSurahChanged();
  emit currentVerseChanged();
  highlightCurrentVerse();
}

void
QuranReader::updatePageVerseInfoList()
{
  if (m_activeQuranBrowser == m_quranBrowsers[0]) {
    m_vLists[0] =
      Verse::fromList(m_quranService->verseInfoList(m_currVerse.page()));
    if (m_config.readerMode() == Configuration::DoublePage)
      m_vLists[1] =
        Verse::fromList(m_quranService->verseInfoList(m_currVerse.page() + 1));

    m_activeVList = &m_vLists[0];

  } else {
    m_vLists[0] =
      Verse::fromList(m_quranService->verseInfoList(m_currVerse.page() - 1));
    m_vLists[1] =
      Verse::fromList(m_quranService->verseInfoList(m_currVerse.page()));

    m_activeVList = &m_vLists[1];
  }
}

void
QuranReader::btnNextClicked()
{
  if (m_config.readerMode() == ReaderMode::SinglePage ||
      m_currVerse.page() % 2 == 0)
    nextPage(1);
  else
    nextPage(2);
}

void
QuranReader::btnPrevClicked()
{
  if (m_config.readerMode() == ReaderMode::SinglePage ||
      m_currVerse.page() % 2 != 0)
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
  int currSurah = m_currVerse.surah();
  m_currVerse.update(v);
  emit currentVerseChanged();

  if (currSurah != v.surah())
    emit currentSurahChanged();
}

void
QuranReader::setVerseToStartOfPage()
{
  // set the current verse to the verse at the top of the page
  m_currVerse.update(m_activeVList->at(0));
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
    senderBrowser->lmbVerseMenu(m_bookmarkService->isBookmarked(v));

  switch (chosenAction) {
    case QuranPageBrowser::Play:
      selectVerse(browerIdx, idx);
      highlightCurrentVerse();
      m_player->play();
      break;
    case QuranPageBrowser::Select:
      selectVerse(browerIdx, idx);
      highlightCurrentVerse();
      break;
    case QuranPageBrowser::Tafsir:
      emit showVerseTafsir(v);
      break;
    case QuranPageBrowser::Translation:
      emit showVerseTranslation(v);
      break;
    case QuranPageBrowser::Thoughts:
      emit showVerseThoughts(v);
      break;
    case QuranPageBrowser::Copy:
      emit copyVerseText(v);
      break;
    case QuranPageBrowser::AddBookmark:
      m_bookmarkService->addBookmark(v, false);
      break;
    case QuranPageBrowser::RemoveBookmark:
      m_bookmarkService->removeBookmark(v, false);
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

  m_currVerse.setNumber(verse);
  emit currentVerseChanged();

  if (m_currVerse.surah() != surah) {
    m_currVerse.setSurah(surah);
    emit currentSurahChanged();
  }

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
    if (m_config.readerMode() == ReaderMode::SinglePage)
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
  m_currVerse.setPage(page);
  redrawQuranPage();

  m_currVerse.update(m_activeVList->at(0));
  addSideContent();
}

void
QuranReader::gotoDoublePage(int page)
{
  int currPage = m_currVerse.page();
  m_currVerse.setPage(page);

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
  if (m_currVerse.page() + step <= 604) {
    gotoPage(m_currVerse.page() + step, true, true);

    if (m_config.readerMode() == ReaderMode::SinglePage)
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
  if (m_currVerse.page() - step >= 1) {
    gotoPage(m_currVerse.page() - step, true, true);

    if (m_config.readerMode() == ReaderMode::SinglePage)
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
  int page = m_quranService->surahStartPage(surahIdx);
  gotoPage(page, false);

  m_currVerse.setPage(page);
  m_currVerse.setSurah(surahIdx);
  m_currVerse.setNumber((surahIdx == 9 || surahIdx == 1) ? 1 : 0);

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
