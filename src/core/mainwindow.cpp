/**
 * @file mainwindow.cpp
 * @brief Implementation file for MainWindow
 */

#include "mainwindow.h"
#include "../widgets/clickablelabel.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_process(new QProcess(this))
  , m_shortcutHandler(new ShortcutHandler(this))
{
  ui->setupUi(this);
  ui->menuView->addAction(ui->sideDock->toggleViewAction());
  ui->frmCenteralCont->setLayoutDirection(Qt::LeftToRight);
  loadIcons();
  loadSettings();
  init();

  if (m_settings->value("WindowState").isNull())
    m_settings->setValue("WindowState", saveState());
  else
    restoreState(m_settings->value("WindowState").toByteArray());

  // connectors
  setupShortcuts();
  setupConnections();
  setupSurahsDock();
  setupMenubarToggle();
  this->show();

  m_notifyMgr->setTooltip(tr("Quran Companion"));
  if (m_settings->value("VOTD").toBool())
    m_notifyMgr->checkDailyVerse();

  m_popup->dockLocationChanged(dockWidgetArea(ui->sideDock));
}

void
MainWindow::loadIcons()
{
  ui->actionDownload_manager->setIcon(
    QIcon(m_resources.filePath("icons/download-manager.png")));
  ui->actionExit->setIcon(QIcon(m_resources.filePath("icons/exit.png")));
  ui->actionFind->setIcon(QIcon(m_resources.filePath("icons/search.png")));
  ui->actionTafsir->setIcon(QIcon(m_resources.filePath("icons/tafsir.png")));
  ui->actionVerse_of_the_day->setIcon(
    QIcon(m_resources.filePath("icons/today.png")));
  ui->actionBookmarks->setIcon(
    QIcon(m_resources.filePath("icons/bookmark-true.png")));
  ui->actionPereferences->setIcon(
    QIcon(m_resources.filePath("icons/prefs.png")));
  ui->btnPlay->setIcon(QIcon(m_resources.filePath("icons/play.png")));
  ui->btnPause->setIcon(QIcon(m_resources.filePath("icons/pause.png")));
  ui->btnStop->setIcon(QIcon(m_resources.filePath("icons/stop.png")));
  ui->actionCheck_for_updates->setIcon(
    QIcon(m_resources.filePath("icons/update.png")));
}

void
MainWindow::loadSettings()
{
  m_settings->beginGroup("Reader");
  m_currVerse.page = m_settings->value("Page").toInt();
  m_currVerse.surah = m_settings->value("Surah").toInt();
  m_currVerse.number = m_settings->value("Verse").toInt();
  m_settings->endGroup();
}

void
MainWindow::loadReader()
{
  if (m_readerMode == ReaderMode::singlePage) {
    m_activeQuranBrowser = m_quranBrowsers[0] =
      new QuranPageBrowser(ui->frmPageContent, m_currVerse.page);

    QWidget* scrollWidget = new QWidget();
    QVBoxLayout* vbl = new QVBoxLayout();
    vbl->setDirection(QBoxLayout::BottomToTop);
    scrollWidget->setLayout(vbl);

    m_scrlVerseByVerse = new QScrollArea;
    m_scrlVerseByVerse->setWidget(scrollWidget);
    m_scrlVerseByVerse->setWidgetResizable(true);
    m_scrlVerseByVerse->setStyleSheet("* { background: transparent }");

    ui->frmSidePanel->layout()->addWidget(m_scrlVerseByVerse);
    ui->centeralSplitter->setStretchFactor(0, 1);
    ui->centeralSplitter->setStretchFactor(1, 0);
  }

  else {
    // even Quran pages are always on the left side
    if (m_currVerse.page % 2 == 0) {
      m_quranBrowsers[0] =
        new QuranPageBrowser(ui->frmPageContent, m_currVerse.page - 1);
      m_activeQuranBrowser = m_quranBrowsers[1] =
        new QuranPageBrowser(ui->frmSidePanel, m_currVerse.page);

    } else {
      m_activeQuranBrowser = m_quranBrowsers[0] =
        new QuranPageBrowser(ui->frmPageContent, m_currVerse.page);
      m_quranBrowsers[1] =
        new QuranPageBrowser(ui->frmSidePanel, m_currVerse.page + 1);
    }

    ui->frmSidePanel->layout()->addWidget(m_quranBrowsers[1]);
    ui->centeralSplitter->insertWidget(0, new QWidget);
    ui->centeralSplitter->addWidget(new QWidget);

    ui->centeralSplitter->setStretchFactor(0, 5);
    ui->centeralSplitter->setStretchFactor(1, 0);
    ui->centeralSplitter->setStretchFactor(2, 0);
    ui->centeralSplitter->setStretchFactor(3, 5);

    ui->centeralSplitter->setHandleWidth(0);
    for (int i = 0; i < 4; i++)
      ui->centeralSplitter->handle(i)->setEnabled(false);
  }

  ui->frmPageContent->layout()->addWidget(m_quranBrowsers[0]);
}

void
MainWindow::checkForUpdates()
{
  QFileInfo tool(m_updateToolPath);
  if (tool.exists()) {
    m_process->setWorkingDirectory(QApplication::applicationDirPath());
    m_process->start(m_updateToolPath, QStringList("ch"));
  } else {
    m_downManPtr->getLatestVersion();
  }
}

void
MainWindow::updateProcessCallback()
{
  QString output = m_process->readAll();
  QString displayText;

  if (output.contains("There are currently no updates available.")) {
    displayText = tr("There are currently no updates available.");

    if (this->isVisible())
      QMessageBox::information(this, tr("Update info"), displayText);
    else
      m_notifyMgr->notify(tr("Update info"), displayText);
  }

  else {
    displayText = tr("Updates available, do you want to open the update tool?");
    if (this->isVisible()) {
      QMessageBox::StandardButton btn =
        QMessageBox::question(this, tr("Updates info"), displayText);
      if (btn == QMessageBox::Yes)
        m_process->startDetached(m_updateToolPath);
    }

    else {
      m_notifyMgr->notify(
        tr("Update info"),
        tr("Updates are available, use the maintainance tool to install "
           "the latest updates."));
    }
  }
}

void
MainWindow::setupShortcuts()
{
  connect(m_shortcutHandler,
          &ShortcutHandler::toggleMenubar,
          this,
          &MainWindow::toggleMenubar,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::toggleNavDock,
          this,
          &MainWindow::toggleNavDock,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::togglePlayback,
          this,
          &MainWindow::togglePlayback,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::incrementVolume,
          this,
          &MainWindow::incrementVolume,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::decrementVolume,
          this,
          &MainWindow::decrementVolume,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::bookmarkCurrent,
          this,
          &MainWindow::addCurrentToBookmarks,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::nextPage,
          this,
          &MainWindow::btnNextClicked,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::prevPage,
          this,
          &MainWindow::btnPrevClicked,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::nextVerse,
          this,
          &MainWindow::nextVerse,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::prevVerse,
          this,
          &MainWindow::prevVerse,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::nextJuz,
          this,
          &MainWindow::nextJuz,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::prevJuz,
          this,
          &MainWindow::prevJuz,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::nextSurah,
          this,
          &MainWindow::nextSurah,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::prevSurah,
          this,
          &MainWindow::prevSurah,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::openDownloads,
          this,
          &MainWindow::actionDMTriggered,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::openBookmarks,
          this,
          &MainWindow::actionBookmarksTriggered,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::openSearch,
          this,
          &MainWindow::actionSearchTriggered,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::openSettings,
          this,
          &MainWindow::actionPrefTriggered,
          Qt::UniqueConnection);
  connect(m_shortcutHandler,
          &ShortcutHandler::openTafsir,
          this,
          &MainWindow::actionTafsirTriggered,
          Qt::UniqueConnection);
}

void
MainWindow::setupConnections()
{

  /* ------------------ UI connectors ------------------ */

  // ########## Menubar ########## //
  connect(ui->actionExit, &QAction::triggered, this, &QApplication::exit);
  connect(ui->actionPereferences,
          &QAction::triggered,
          this,
          &MainWindow::actionPrefTriggered,
          Qt::UniqueConnection);
  connect(ui->actionDownload_manager,
          &QAction::triggered,
          this,
          &MainWindow::actionDMTriggered,
          Qt::UniqueConnection);
  connect(ui->actionFind,
          &QAction::triggered,
          this,
          &MainWindow::actionSearchTriggered,
          Qt::UniqueConnection);
  connect(ui->actionTafsir,
          &QAction::triggered,
          this,
          &MainWindow::actionTafsirTriggered,
          Qt::UniqueConnection);
  connect(ui->actionVerse_of_the_day,
          &QAction::triggered,
          this,
          &MainWindow::actionVotdTriggered,
          Qt::UniqueConnection);
  connect(ui->actionCheck_for_updates,
          &QAction::triggered,
          this,
          &MainWindow::checkForUpdates,
          Qt::UniqueConnection);
  connect(m_process,
          &QProcess::finished,
          this,
          &MainWindow::updateProcessCallback,
          Qt::UniqueConnection);
  connect(ui->actionBookmarks,
          &QAction::triggered,
          this,
          &MainWindow::actionBookmarksTriggered,
          Qt::UniqueConnection);
  connect(ui->actionAbout_Quran_Companion,
          &QAction::triggered,
          this,
          &MainWindow::actionAboutTriggered,
          Qt::UniqueConnection);

  // ########## Quran page ########## //
  connect(m_quranBrowsers[0],
          &QTextBrowser::anchorClicked,
          this,
          &MainWindow::verseAnchorClicked);
  if (m_quranBrowsers[1]) {
    connect(m_quranBrowsers[1],
            &QTextBrowser::anchorClicked,
            this,
            &MainWindow::verseAnchorClicked);
  }

  // ########## page controls ########## //
  connect(ui->btnNext,
          &QPushButton::clicked,
          this,
          &MainWindow::btnNextClicked,
          Qt::UniqueConnection);
  connect(ui->btnPrev,
          &QPushButton::clicked,
          this,
          &MainWindow::btnPrevClicked,
          Qt::UniqueConnection);
  connect(ui->cmbPage,
          &QComboBox::currentIndexChanged,
          this,
          &MainWindow::cmbPageChanged,
          Qt::UniqueConnection);
  connect(ui->cmbVerse,
          &QComboBox::currentIndexChanged,
          this,
          &MainWindow::cmbVerseChanged,
          Qt::UniqueConnection);
  connect(ui->cmbJuz,
          &QComboBox::currentIndexChanged,
          this,
          &MainWindow::cmbJuzChanged,
          Qt::UniqueConnection);
  connect(m_player,
          &VersePlayer::missingVerseFile,
          this,
          &MainWindow::missingRecitationFileWarn,
          Qt::UniqueConnection);

  // ########## navigation dock ########## //
  connect(ui->lineEditSearchSurah,
          &QLineEdit::textChanged,
          this,
          &MainWindow::searchSurahTextChanged,
          Qt::UniqueConnection);
  connect(ui->listViewSurahs,
          &QListView::clicked,
          this,
          &MainWindow::listSurahNameClicked,
          Qt::UniqueConnection);

  // ########## audio slider ########## //
  connect(m_player,
          &QMediaPlayer::positionChanged,
          this,
          &MainWindow::mediaPosChanged,
          Qt::UniqueConnection);
  connect(m_player,
          &QMediaPlayer::playbackStateChanged,
          this,
          &MainWindow::mediaStateChanged,
          Qt::UniqueConnection);
  connect(ui->sldrAudioPlayer,
          &QSlider::sliderMoved,
          m_player,
          &QMediaPlayer::setPosition,
          Qt::UniqueConnection);
  connect(ui->sldrVolume,
          &QSlider::valueChanged,
          this,
          &MainWindow::volumeSliderValueChanged,
          Qt::UniqueConnection);

  // ########## player control ########## //
  connect(m_player,
          &QMediaPlayer::mediaStatusChanged,
          this,
          &MainWindow::mediaStatusChanged,
          Qt::UniqueConnection);
  connect(ui->btnPlay,
          &QPushButton::clicked,
          this,
          &MainWindow::btnPlayClicked,
          Qt::UniqueConnection);
  connect(ui->btnPause,
          &QPushButton::clicked,
          this,
          &MainWindow::btnPauseClicked,
          Qt::UniqueConnection);
  connect(ui->btnStop,
          &QPushButton::clicked,
          this,
          &MainWindow::btnStopClicked,
          Qt::UniqueConnection);
  connect(ui->cmbReciter,
          &QComboBox::currentIndexChanged,
          m_player,
          &VersePlayer::changeReciter,
          Qt::UniqueConnection);

  // ########## system tray ########## //
  connect(m_notifyMgr, &NotificationManager::exit, this, &QApplication::exit);
  connect(m_notifyMgr,
          &NotificationManager::togglePlayback,
          this,
          &MainWindow::togglePlayback,
          Qt::UniqueConnection);
  connect(m_notifyMgr,
          &NotificationManager::showWindow,
          this,
          &MainWindow::show,
          Qt::UniqueConnection);
  connect(m_notifyMgr,
          &NotificationManager::hideWindow,
          this,
          &MainWindow::hide,
          Qt::UniqueConnection);
  connect(m_notifyMgr,
          &NotificationManager::checkForUpdates,
          this,
          &MainWindow::checkForUpdates,
          Qt::UniqueConnection);
  connect(m_notifyMgr,
          &NotificationManager::openAbout,
          this,
          &MainWindow::actionAboutTriggered,
          Qt::UniqueConnection);
  connect(m_notifyMgr,
          &NotificationManager::showVOTDmessagebox,
          this,
          &MainWindow::showVOTDmessage,
          Qt::UniqueConnection);
  connect(m_notifyMgr,
          &NotificationManager::openPrefs,
          this,
          &MainWindow::actionPrefTriggered,
          Qt::UniqueConnection);

  // ########## Notification Popup ########## //
  connect(ui->sideDock,
          &QDockWidget::dockLocationChanged,
          m_popup,
          &NotificationPopup::dockLocationChanged,
          Qt::UniqueConnection);
  connect(m_downManPtr,
          &DownloadManager::downloadComplete,
          m_popup,
          &NotificationPopup::completedDownload,
          Qt::UniqueConnection);
  connect(m_downManPtr,
          &DownloadManager::downloadError,
          m_popup,
          &NotificationPopup::downloadError,
          Qt::UniqueConnection);
  connect(m_downManPtr,
          &DownloadManager::latestVersionFound,
          m_popup,
          &NotificationPopup::checkUpdate,
          Qt::UniqueConnection);
}

void
MainWindow::init()
{
  m_player =
    new VersePlayer(this, m_currVerse, m_settings->value("Reciter", 0).toInt());
  m_popup = new NotificationPopup(this);
  m_downManPtr = new DownloadManager(this);

  loadReader();

  ui->cmbPage->setValidator(new QIntValidator(1, 604, this));
  m_notifyMgr = new NotificationManager(this);

  updateLoadedTafsir();
  updateLoadedTranslation();
  updateSideFont();

  redrawQuranPage(true);
  setVerseComboBoxRange(true);

  if (m_readerMode == ReaderMode::singlePage)
    addSideContent();

  for (int i = 1; i < 605; i++) {
    ui->cmbPage->addItem(QString::number(i));
  }

  foreach (const Reciter& r, m_recitersList) {
    ui->cmbReciter->addItem(r.displayName);
  }

  // sets without emitting signal
  setCmbVerseIdx(m_currVerse.number - 1);
  setCmbJuzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);

  ui->cmbPage->setCurrentIndex(m_currVerse.page - 1);
  ui->cmbReciter->setCurrentIndex(m_settings->value("Reciter", 0).toInt());
}

void
MainWindow::setupSurahsDock()
{
  for (int i = 1; i < 115; i++) {
    QString item = QString::number(i).rightJustified(3, '0') + ' ' +
                   m_dbMgr->surahNameList().at(i - 1);
    m_surahList.append(item);
  }

  m_surahListModel.setStringList(m_surahList);
  ui->listViewSurahs->setModel(&m_surahListModel);

  QItemSelectionModel* selector = ui->listViewSurahs->selectionModel();
  selector->select(m_surahListModel.index(m_currVerse.surah - 1),
                   QItemSelectionModel::Rows | QItemSelectionModel::Select);

  ui->listViewSurahs->scrollTo(m_surahListModel.index(m_currVerse.surah - 1),
                               QAbstractItemView::PositionAtCenter);
}

void
MainWindow::setupMenubarToggle()
{
  QPushButton* toggleNav = new QPushButton(this);
  toggleNav->setObjectName("btnToggleNav");
  toggleNav->setCheckable(true);
  toggleNav->setChecked(!ui->sideDock->isHidden());
  toggleNav->setCursor(Qt::PointingHandCursor);
  toggleNav->setToolTip(tr("Navigation"));
  toggleNav->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  toggleNav->setMinimumSize(
    QSize(ui->menubar->height() + 10, ui->menubar->height()));
  ui->menubar->setCornerWidget(toggleNav);

  connect(toggleNav, &QPushButton::toggled, this, [this](bool checked) {
    if (ui->sideDock->isVisible() != checked)
      ui->sideDock->setVisible(checked);
  });

  connect(ui->sideDock->toggleViewAction(),
          &QAction::toggled,
          toggleNav,
          [this, toggleNav](bool visible) {
            if (toggleNav->isChecked() != visible && !this->isMinimized())
              toggleNav->setChecked(visible);
          });
}

bool
MainWindow::areNeighbors(int page1, int page2)
{
  return page1 % 2 != 0 && page2 % 2 == 0 && page2 == page1 + 1;
}

void
MainWindow::switchActivePage()
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
MainWindow::btnNextClicked()
{
  if (m_readerMode == ReaderMode::singlePage || m_currVerse.page % 2 == 0)
    nextPage(1);
  else
    nextPage(2);
}

void
MainWindow::btnPrevClicked()
{
  if (m_readerMode == ReaderMode::singlePage || m_currVerse.page % 2 != 0)
    prevPage(1);
  else
    prevPage(2);
}

void
MainWindow::selectVerse(int browserIdx, int IdxInPage)
{
  if (m_activeQuranBrowser != m_quranBrowsers[browserIdx])
    switchActivePage();

  const Verse& v = m_vLists[browserIdx].at(IdxInPage);
  int currSurah = m_currVerse.surah;
  m_currVerse = v;
  m_player->setVerse(m_currVerse);
  if (currSurah != v.surah) {
    setVerseComboBoxRange();
    syncSelectedSurah();
  } else
    setCmbVerseIdx(v.number - 1);

  setCmbPageIdx(v.page - 1);
  setCmbJuzIdx(m_dbMgr->getJuzOfPage(v.page) - 1);
}

void
MainWindow::updateSurahVerseCount()
{

  m_surahCount = m_dbMgr->getSurahVerseCount(m_currVerse.surah);
}

QModelIndex
MainWindow::syncSelectedSurah()
{
  QItemSelectionModel* select = ui->listViewSurahs->selectionModel();
  select->clearSelection();
  QModelIndex surah = m_surahListModel.index(m_currVerse.surah - 1);
  select->select(surah,
                 QItemSelectionModel::Rows | QItemSelectionModel::Select);
  ui->listViewSurahs->scrollTo(surah, QAbstractItemView::PositionAtCenter);

  return surah;
}

void
MainWindow::updatePageVerseInfoList()
{
  if (m_activeQuranBrowser == m_quranBrowsers[0]) {
    m_vLists[0] = m_dbMgr->getVerseInfoList(m_currVerse.page);
    if (m_readerMode == doublePage)
      m_vLists[1] = m_dbMgr->getVerseInfoList(m_currVerse.page + 1);

    m_activeVList = &m_vLists[0];

  } else {
    m_vLists[0] = m_dbMgr->getVerseInfoList(m_currVerse.page - 1);
    m_vLists[1] = m_dbMgr->getVerseInfoList(m_currVerse.page);

    m_activeVList = &m_vLists[1];
  }
}

void
MainWindow::setVerseToStartOfPage()
{
  // set the current verse to the verse at the top of the page
  m_currVerse = m_activeVList->at(0);
  // update the player active verse
  m_player->setVerse(m_currVerse);
  // open newly set verse recitation file
  m_player->loadActiveVerse();
}

void
MainWindow::setCmbPageIdx(int idx)
{
  m_internalPageChange = true;
  ui->cmbPage->setCurrentIndex(idx);
  m_internalPageChange = false;
}

void
MainWindow::setCmbVerseIdx(int idx)
{
  // idx == -1 if playing basmallah, therefore highlight 1st verse
  if (idx < 0)
    idx = 0;

  m_internalVerseChange = true;
  ui->cmbVerse->setCurrentIndex(idx);
  m_internalVerseChange = false;
}

void
MainWindow::setCmbJuzIdx(int idx)
{
  m_internalJuzChange = true;
  ui->cmbJuz->setCurrentIndex(idx);
  m_internalJuzChange = false;
}

void
MainWindow::setVerseComboBoxRange(bool forceUpdate)
{
  int oldCount = m_surahCount;
  updateSurahVerseCount();

  if (m_surahCount != oldCount || forceUpdate) {
    if (m_verseValidator != nullptr)
      delete m_verseValidator;
    m_verseValidator = new QIntValidator(1, m_surahCount, ui->cmbVerse);

    // updates values in the combobox with the current surah verses
    ui->cmbVerse->clear();
    m_internalVerseChange = true;
    for (int i = 1; i <= m_surahCount; i++)
      ui->cmbVerse->addItem(QString::number(i), i);
    m_internalVerseChange = false;

    ui->cmbVerse->setValidator(m_verseValidator);
  }

  setCmbVerseIdx(m_currVerse.number - 1);
}

void
MainWindow::gotoPage(int page, bool updateElements, bool automaticFlip)
{
  m_activeQuranBrowser->resetHighlight();

  if (!automaticFlip)
    m_player->stop();

  if (m_activeQuranBrowser->page() != page) {
    if (m_readerMode == ReaderMode::singlePage)
      gotoSinglePage(page);
    else
      gotoDoublePage(page);
  }

  if (updateElements) {
    setVerseToStartOfPage();
    syncSelectedSurah();
    setVerseComboBoxRange();
    setCmbJuzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);
    setCmbPageIdx(m_currVerse.page - 1);
  }
}

void
MainWindow::gotoSinglePage(int page)
{
  m_currVerse.page = page;
  redrawQuranPage();

  m_currVerse = m_activeVList->at(0);
  addSideContent();
}

void
MainWindow::gotoDoublePage(int page)
{
  int currPage = m_currVerse.page;
  m_currVerse.page = page;

  int pageBrowerIdx = page % 2 == 0;

  if (areNeighbors(currPage, page) || areNeighbors(page, currPage))
    switchActivePage();
  else {
    m_activeQuranBrowser = m_quranBrowsers[pageBrowerIdx];
    redrawQuranPage();
  }
}

void
MainWindow::nextPage(int step)
{
  bool keepPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
  if (m_currVerse.page + step <= 604) {
    setCmbPageIdx(m_currVerse.page + step - 1);

    gotoPage(m_currVerse.page + step, true, true);

    if (m_readerMode == ReaderMode::singlePage)
      m_scrlVerseByVerse->verticalScrollBar()->setValue(0);

    // if the page is flipped automatically, resume playback
    if (keepPlaying) {
      m_player->play();
      highlightCurrentVerse();
    }
  }
}

void
MainWindow::prevPage(int step)
{
  bool keepPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
  if (m_currVerse.page - step >= 1) {
    setCmbPageIdx(m_currVerse.page - step - 1);

    gotoPage(m_currVerse.page - step, true, true);

    if (m_readerMode == ReaderMode::singlePage)
      m_scrlVerseByVerse->verticalScrollBar()->setValue(0);

    if (keepPlaying) {
      m_player->play();
      highlightCurrentVerse();
    }
  }
}

void
MainWindow::gotoSurah(int surahIdx)
{
  // getting surah index
  int page = m_dbMgr->getSurahStartPage(surahIdx);
  gotoPage(page, false);

  m_currVerse.page = page;
  m_currVerse.surah = surahIdx;
  m_currVerse.number = surahIdx == 9 || surahIdx == 1 ? 1 : 0;

  // syncing the player & playing basmalah
  m_player->setVerse(m_currVerse);
  m_player->playCurrentVerse();

  highlightCurrentVerse();
  setCmbPageIdx(m_currVerse.page - 1);
  setCmbJuzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);
  setVerseComboBoxRange();
  syncSelectedSurah();
}

void
MainWindow::searchSurahTextChanged(const QString& arg1)
{
  if (arg1.isEmpty()) {
    m_surahListModel.setStringList(m_surahList);
    syncSelectedSurah();
  } else {
    QList<int> suggestions = m_dbMgr->searchSurahNames(arg1);
    QStringList res;
    foreach (int sura, suggestions)
      res.append(m_surahList.at(sura - 1));

    m_surahListModel.setStringList(res);
  }
}

void
MainWindow::listSurahNameClicked(const QModelIndex& index)
{
  int s = m_surahList.indexOf(index.data().toString()) + 1;
  gotoSurah(s);
}

void
MainWindow::cmbPageChanged(int newIdx)
{
  if (m_internalPageChange) {
    qDebug() << "Internal page change";
    return;
  }

  gotoPage(newIdx + 1);
}

void
MainWindow::cmbVerseChanged(int newVerseIdx)
{
  if (newVerseIdx < 0)
    return;

  if (m_internalVerseChange) {
    qDebug() << "internal verse change";
    return;
  }

  int verse = newVerseIdx + 1;
  int page = m_dbMgr->getVersePage(m_currVerse.surah, verse);

  if (page != m_currVerse.page)
    gotoPage(page, false);

  m_currVerse.page = page;
  m_currVerse.number = verse;
  highlightCurrentVerse();

  setCmbPageIdx(page - 1);
  setCmbJuzIdx(m_dbMgr->getJuzOfPage(page) - 1);

  // open newly set verse recitation file
  m_player->setVerse(m_currVerse);
  m_player->loadActiveVerse();
}

void
MainWindow::cmbJuzChanged(int newJuzIdx)
{
  if (m_internalJuzChange) {
    qDebug() << "Internal jozz change";
    return;
  }
  int page = m_dbMgr->getJuzStartPage(newJuzIdx + 1);
  gotoPage(page);
}

void
MainWindow::togglePlayback()
{
  if (m_player->playbackState() == QMediaPlayer::PlayingState) {
    btnPauseClicked();
  } else {
    btnPlayClicked();
  }
}

void
MainWindow::btnPauseClicked()
{
  m_player->pause();
}

void
MainWindow::btnPlayClicked()
{
  highlightCurrentVerse();
  m_player->play();
}

void
MainWindow::btnStopClicked()
{
  m_player->stop();
  setVerseToStartOfPage();
  syncSelectedSurah();
  setVerseComboBoxRange();
}

void
MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
  if (status == QMediaPlayer::EndOfMedia) {
    nextVerse();
  }
}

void
MainWindow::mediaStateChanged(QMediaPlayer::PlaybackState state)
{
  if (state == QMediaPlayer::PlayingState) {
    ui->btnPlay->setEnabled(false);
    ui->btnPause->setEnabled(true);
    ui->btnStop->setEnabled(true);
  } else if (state == QMediaPlayer::PausedState) {
    ui->btnPlay->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(true);
  } else if (state == QMediaPlayer::StoppedState) {
    ui->btnPlay->setEnabled(true);
    ui->btnPause->setEnabled(false);
    ui->btnStop->setEnabled(false);
  }

  updateTrayTooltip(state);
}

void
MainWindow::addCurrentToBookmarks()
{
  if (!m_dbMgr->isBookmarked(m_currVerse)) {
    m_dbMgr->addBookmark(m_currVerse);
    m_popup->bookmarkAdded();
  }
}

void
MainWindow::mediaPosChanged(qint64 position)
{
  if (ui->sldrAudioPlayer->maximum() != m_player->duration())
    ui->sldrAudioPlayer->setMaximum(m_player->duration());

  if (!ui->sldrAudioPlayer->isSliderDown())
    ui->sldrAudioPlayer->setValue(position);
}

void
MainWindow::volumeSliderValueChanged(int position)
{
  qreal linearVolume =
    QAudio::convertVolume(ui->sldrVolume->value() / qreal(100.0),
                          QAudio::LogarithmicVolumeScale,
                          QAudio::LinearVolumeScale);
  if (linearVolume != m_volume) {
    m_volume = linearVolume;
    m_player->setPlayerVolume(m_volume);
  }
}

void
MainWindow::missingRecitationFileWarn(int reciterIdx, int surah)
{
  if (!m_settings->value("MissingFileWarning").toBool())
    return;

  QMessageBox::StandardButton btn =
    QMessageBox::question(this,
                          tr("Recitation not found"),
                          tr("The recitation files for the current surah is "
                             "missing, would you like to download it?"));

  if (btn == QMessageBox::Yes) {
    actionDMTriggered();
    m_downloaderDlg->selectTask(reciterIdx, surah);
  }
}

void
MainWindow::verseClicked()
{
  // object = clickable label, parent = verse frame, verse frame name scheme =
  // 'surah_verse'
  QStringList data = sender()->parent()->objectName().split('_');
  int surah = data.at(0).toInt();
  int verse = data.at(1).toInt();

  m_currVerse.number = verse;
  m_player->setVerse(m_currVerse);

  if (m_currVerse.surah != surah) {
    m_currVerse.surah = surah;
    m_player->setVerse(m_currVerse);
    setVerseComboBoxRange();
    syncSelectedSurah();
  }

  setCmbVerseIdx(verse - 1);
  m_player->loadActiveVerse();
  btnPlayClicked();
}

void
MainWindow::verseAnchorClicked(const QUrl& hrefUrl)
{
  QuranPageBrowser* senderBrowser = qobject_cast<QuranPageBrowser*>(sender());
  int browerIdx = senderBrowser == m_quranBrowsers[1];
  int idx = hrefUrl.toString().remove('#').toInt();
  Verse v = m_vLists[browerIdx].at(idx);

  QuranPageBrowser::Action chosenAction =
    senderBrowser->lmbVerseMenu(m_dbMgr->isBookmarked(v));

  switch (chosenAction) {
    case QuranPageBrowser::play:
      selectVerse(browerIdx, idx);
      m_player->loadActiveVerse();
      btnPlayClicked();
      break;
    case QuranPageBrowser::select:
      selectVerse(browerIdx, idx);
      m_player->setSource(QUrl());
      highlightCurrentVerse();
      break;

    case QuranPageBrowser::tafsir:
      showVerseTafsir(v);
      break;
    case QuranPageBrowser::copy:
      copyVerseText(v);
      m_popup->copiedToClipboard();
      break;
    case QuranPageBrowser::addBookmark:
      m_dbMgr->addBookmark(v);
      m_popup->bookmarkAdded();
      break;
    case QuranPageBrowser::removeBookmark:
      if (m_dbMgr->removeBookmark(v))
        m_popup->bookmarkRemoved();
      break;
    default:
      break;
  }
}

void
MainWindow::actionPrefTriggered()
{
  if (m_settingsDlg == nullptr) {
    m_settingsDlg = new SettingsDialog(this, m_player);

    // Restart signal
    connect(m_settingsDlg,
            &SettingsDialog::restartApp,
            this,
            &MainWindow::restartApp,
            Qt::UniqueConnection);

    // Quran page signals
    connect(m_settingsDlg,
            &SettingsDialog::redrawQuranPage,
            this,
            &MainWindow::redrawQuranPage,
            Qt::UniqueConnection);
    connect(m_settingsDlg,
            &SettingsDialog::quranFontChanged,
            m_quranBrowsers[0],
            &QuranPageBrowser::updateFontSize,
            Qt::UniqueConnection);
    if (m_quranBrowsers[1])
      connect(m_settingsDlg,
              &SettingsDialog::quranFontChanged,
              m_quranBrowsers[1],
              &QuranPageBrowser::updateFontSize,
              Qt::UniqueConnection);

    // Side panel signals
    connect(m_settingsDlg,
            &SettingsDialog::redrawSideContent,
            this,
            &MainWindow::addSideContent,
            Qt::UniqueConnection);
    connect(m_settingsDlg,
            &SettingsDialog::tafsirChanged,
            this,
            &MainWindow::updateLoadedTafsir,
            Qt::UniqueConnection);
    connect(m_settingsDlg,
            &SettingsDialog::translationChanged,
            this,
            &MainWindow::updateLoadedTranslation,
            Qt::UniqueConnection);
    connect(m_settingsDlg,
            &SettingsDialog::sideFontChanged,
            this,
            &MainWindow::updateSideFont,
            Qt::UniqueConnection);

    // audio device signals
    connect(m_settingsDlg,
            &SettingsDialog::usedAudioDeviceChanged,
            m_player,
            &VersePlayer::changeUsedAudioDevice,
            Qt::UniqueConnection);

    // shortcut change
    connect(m_settingsDlg,
            &SettingsDialog::shortcutChanged,
            m_shortcutHandler,
            &ShortcutHandler::shortcutChanged,
            Qt::UniqueConnection);
  }

  m_settingsDlg->showWindow();
}

void
MainWindow::actionDMTriggered()
{
  if (m_downloaderDlg == nullptr)
    m_downloaderDlg = new DownloaderDialog(this, m_downManPtr);

  m_downloaderDlg->show();
}

void
MainWindow::actionBookmarksTriggered()
{
  if (m_bookmarksDlg == nullptr) {
    m_bookmarksDlg = new BookmarksDialog(this);
    connect(m_bookmarksDlg,
            &BookmarksDialog::navigateToVerse,
            this,
            &MainWindow::navigateToVerse,
            Qt::UniqueConnection);
  }

  m_bookmarksDlg->showWindow();
}

void
MainWindow::actionTafsirTriggered()
{
  showVerseTafsir(m_currVerse);
}

void
MainWindow::actionVotdTriggered()
{
  showVOTDmessage(m_notifyMgr->votd());
}

void
MainWindow::actionAboutTriggered()
{
  QFile about(":/resources/about.html");
  QString text;
  if (about.open(QIODevice::ReadOnly))
    text = about.readAll();
  about.close();

  text = text.arg(tr("Quran Companion v"))
           .arg(QApplication::applicationVersion(),
                tr("Quran Companion"),
                tr(" is a free cross-platform Quran reader & player."),
                tr("Licensed under the "),
                tr("'Waqf' General Public License"),
                tr("Recitations"),
                tr("Tafsir/Translations"));

  QMessageBox::about(this, tr("About Quran Companion"), text);
}

void
MainWindow::on_actionAbout_Qt_triggered()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

void
MainWindow::actionSearchTriggered()
{
  if (m_searchDlg == nullptr) {
    m_searchDlg = new SearchDialog(this);
    connect(m_searchDlg,
            &SearchDialog::navigateToVerse,
            this,
            &MainWindow::navigateToVerse);
  }

  m_searchDlg->show();
}

void
MainWindow::navigateToVerse(Verse v)
{
  gotoPage(v.page, false);

  m_currVerse = v;
  setVerseComboBoxRange();

  setCmbPageIdx(m_currVerse.page - 1);
  setCmbVerseIdx(m_currVerse.number - 1);
  setCmbJuzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);

  syncSelectedSurah();
  highlightCurrentVerse();

  m_player->setVerse(m_currVerse);
  m_player->loadActiveVerse();
}

void
MainWindow::navigateToSurah(QModelIndex& index)
{
  int s = index.row() + 1;
  gotoSurah(s);
}

void
MainWindow::updateLoadedTafsir()
{
  Tafsir currTafsir = qvariant_cast<Tafsir>(m_settings->value("Reader/Tafsir"));

  m_dbMgr->setCurrentTafsir(currTafsir);
}

void
MainWindow::updateLoadedTranslation()
{
  Translation currTrans =
    qvariant_cast<Translation>(m_settings->value("Reader/Translation"));

  m_dbMgr->setCurrentTranslation(currTrans);
}

void
MainWindow::updateSideFont()
{
  m_sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
}

Verse
MainWindow::incrementVerse()
{
  Verse v = m_currVerse;
  if (v == m_activeVList->last()) {
    v.page++;
  }

  if (v.number < m_surahCount) {
    v.number++;
  } else if (v.number == m_surahCount) {
    v.surah++;
    v.number = v.surah == 9 || v.surah == 1 ? 1 : 0;
  }

  return v;
}

Verse
MainWindow::decrementVerse()
{
  Verse v = m_currVerse;
  if (v.number == 0)
    v.number = 1;

  if (v == m_activeVList->at(0)) {
    v.page--;
  }

  if (v.number > 1) {
    v.number--;
  } else if (v.number <= 1) {
    v.surah--;
    v.number = m_dbMgr->getSurahVerseCount(v.surah);
  }

  return v;
}

void
MainWindow::nextVerse()
{
  if (m_currVerse.surah == 114 && m_currVerse.number == 6)
    return;

  bool keepPlaying = m_player->isOn();
  navigateToVerse(incrementVerse());
  if (keepPlaying)
    m_player->play();
}

void
MainWindow::prevVerse()
{
  if (m_currVerse.surah == 1 && m_currVerse.number == 1)
    return;

  bool keepPlaying = m_player->isOn();
  navigateToVerse(decrementVerse());
  if (keepPlaying)
    m_player->play();
}

void
MainWindow::nextJuz()
{
  if (ui->cmbJuz->currentIndex() != 29)
    ui->cmbJuz->setCurrentIndex(ui->cmbJuz->currentIndex() + 1);
}

void
MainWindow::prevJuz()
{
  if (ui->cmbJuz->currentIndex() != 0)
    ui->cmbJuz->setCurrentIndex(ui->cmbJuz->currentIndex() - 1);
}

void
MainWindow::nextSurah()
{
  if (m_currVerse.surah < 114)
    gotoSurah(m_currVerse.surah + 1);
}

void
MainWindow::prevSurah()
{
  if (m_currVerse.surah > 1)
    gotoSurah(m_currVerse.surah - 1);
}

void
MainWindow::incrementVolume()
{
  int val = ui->sldrVolume->value() + 5;
  ui->sldrVolume->setValue(val > 100 ? 100 : val);
}

void
MainWindow::decrementVolume()
{
  int val = ui->sldrVolume->value() - 5;
  ui->sldrVolume->setValue(val < 0 ? 0 : val);
}

void
MainWindow::toggleMenubar()
{
  if (ui->menubar->isVisible())
    ui->menubar->hide();
  else
    ui->menubar->show();
}

void
MainWindow::toggleNavDock()
{
  ui->sideDock->toggleViewAction()->toggle();
}

void
MainWindow::redrawQuranPage(bool manualSz)
{
  if (m_activeQuranBrowser == m_quranBrowsers[0]) {
    m_quranBrowsers[0]->constructPage(m_currVerse.page, manualSz);
    if (m_readerMode == doublePage && m_quranBrowsers[1])
      m_quranBrowsers[1]->constructPage(m_currVerse.page + 1, manualSz);
  } else {
    m_quranBrowsers[0]->constructPage(m_currVerse.page - 1, manualSz);
    m_quranBrowsers[1]->constructPage(m_currVerse.page, manualSz);
  }

  updatePageVerseInfoList();
}

void
MainWindow::highlightCurrentVerse()
{
  if (m_currVerse.number == 0)
    return;

  // idx may be -1 if verse number is 0 (basmallah)
  int idx = m_activeVList->indexOf(m_currVerse);
  if (idx < 0)
    idx = 0;

  m_activeQuranBrowser->highlightVerse(idx);

  if (m_readerMode == ReaderMode::singlePage)
    setHighlightedFrame();
}

void
MainWindow::setHighlightedFrame()
{
  if (m_highlightedFrm != nullptr)
    m_highlightedFrm->setSelected(false);

  VerseFrame* verseFrame =
    m_scrlVerseByVerse->widget()->findChild<VerseFrame*>(QString("%0_%1").arg(
      QString::number(m_currVerse.surah), QString::number(m_currVerse.number)));

  verseFrame->setSelected(true);

  m_scrlVerseByVerse->ensureWidgetVisible(verseFrame);
  m_highlightedFrm = verseFrame;
}

void
MainWindow::addSideContent()
{
  if (m_readerMode != ReaderMode::singlePage)
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
  for (int i = m_activeVList->size() - 1; i >= 0; i--) {
    Verse vInfo = m_activeVList->at(i);

    verseContFrame = new VerseFrame(m_scrlVerseByVerse->widget());
    verselb = new ClickableLabel(verseContFrame);
    contentLb = new QLabel(verseContFrame);

    verseContFrame->setObjectName(
      QString("%0_%1").arg(vInfo.surah).arg(vInfo.number));

    verselb->setFont(QFont(m_activeQuranBrowser->pageFont(),
                           m_activeQuranBrowser->fontSize() - 2));
    verselb->setText(m_dbMgr->getVerseGlyphs(vInfo.surah, vInfo.number));
    verselb->setAlignment(Qt::AlignCenter);
    verselb->setWordWrap(true);

    currLbContent = m_dbMgr->getTranslation(vInfo.surah, vInfo.number);

    if (currLbContent == prevLbContent) {
      currLbContent = '-';
    } else {
      prevLbContent = currLbContent;
    }

    contentLb->setText(currLbContent);
    contentLb->setTextFormat(Qt::RichText);
    contentLb->setTextInteractionFlags(Qt::TextSelectableByMouse);
    contentLb->setAlignment(Qt::AlignCenter);
    contentLb->setWordWrap(true);
    contentLb->setFont(m_sideFont);

    verseContFrame->layout()->addWidget(verselb);
    verseContFrame->layout()->addWidget(contentLb);
    m_scrlVerseByVerse->widget()->layout()->addWidget(verseContFrame);
    m_verseFrameList.insert(0, verseContFrame);

    // connect clicked signal for each label
    connect(verselb,
            &ClickableLabel::clicked,
            this,
            &MainWindow::verseClicked,
            Qt::UniqueConnection);
  }

  if (m_player->playbackState() == QMediaPlayer::PlayingState) {
    setHighlightedFrame();
  }
}

void
MainWindow::updateTrayTooltip(QMediaPlayer::PlaybackState state)
{
  if (state == QMediaPlayer::PlayingState) {
    m_notifyMgr->setTooltip(tr("Now playing: ") + m_player->reciterName() +
                            " - " + tr("Surah ") +
                            m_dbMgr->getSurahName(m_currVerse.surah));
  } else
    m_notifyMgr->setTooltip(tr("Quran Companion"));
}

void
MainWindow::showVerseTafsir(Verse v)
{
  if (m_tafsirDlg == nullptr) {
    m_tafsirDlg = new TafsirDialog(this);
  }

  m_tafsirDlg->setShownVerse(v);
  m_tafsirDlg->loadVerseTafsir();
  m_tafsirDlg->show();
}

void
MainWindow::copyVerseText(const Verse v)
{
  QClipboard* clip = QApplication::clipboard();
  QString text = m_dbMgr->getVerseText(v.surah, v.number);
  QString vNum = QString::number(v.number);
  text.remove(text.size() - 1, 1);
  text = text.trimmed();
  text = "{" + text + "}";
  text += ' ';
  text += "[" + m_dbMgr->surahNameList().at(v.surah - 1) + ":" + vNum + "]";
  clip->setText(text);
}

void
MainWindow::showVOTDmessage(QPair<Verse, QString> votd)
{
  QPointer<QDialog> mbox = new QDialog(this);
  mbox->setLayout(new QVBoxLayout);
  mbox->setStyleSheet(
    "QDialog:hover{ background-color: rgba(0, 161, 185, 40); }");
  mbox->setWindowIcon(QIcon(m_resources.filePath("/icons/today.png")));
  mbox->setWindowTitle(tr("Verse Of The Day"));
  ClickableLabel* lb = new ClickableLabel(mbox);
  lb->setText(votd.second);
  lb->setTextFormat(Qt::RichText);
  lb->setAlignment(Qt::AlignCenter);
  QStringList uiFonts;
  uiFonts << "Noto Sans Display"
          << "PakType Naskh Basic";
  lb->setFont(QFont(uiFonts, 15));
  lb->setCursor(Qt::PointingHandCursor);
  if (votd.second.length() > 200)
    lb->setWordWrap(true);

  connect(lb, &ClickableLabel::clicked, this, [votd, this, mbox]() {
    mbox->close();
    navigateToVerse(votd.first);
  });

  mbox->layout()->addWidget(lb);
  mbox->show();
}

void
MainWindow::saveReaderState()
{
  m_settings->setValue("WindowState", saveState());
  m_settings->setValue("Reciter", ui->cmbReciter->currentIndex());

  m_settings->beginGroup("Reader");
  m_settings->setValue("Page", m_currVerse.page);
  m_settings->setValue("Surah", m_currVerse.surah);
  m_settings->setValue("Verse",
                       m_currVerse.number == 0 ? 1 : m_currVerse.number);
  m_settings->endGroup();

  m_settings->sync();
}

void
MainWindow::restartApp()
{
  saveReaderState();
  QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
  emit QApplication::exit();
}

void
MainWindow::resizeEvent(QResizeEvent* event)
{
  QMainWindow::resizeEvent(event);
  if (m_popup) {
    m_popup->adjustLocation();
    m_popup->move(m_popup->notificationPos());
  }
}

MainWindow::~MainWindow()
{
  saveReaderState();
  delete ui;
}
