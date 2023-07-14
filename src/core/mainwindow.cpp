#include "mainwindow.h"
#include "../widgets/clickablelabel.h"
#include "ui_mainwindow.h"
/*!
 * \brief MainWindow::MainWindow initalizes the main application window and sets
 * up UI connections.
 *
 * @param parent is a pointer to the parent widget
 * @param settingsPtr is a pointer to the QSettings object to acess app settings
 */
MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_process{ new QProcess(this) }
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
  setupConnections();
  setupSurahsDock();
  setupMenubarToggle();
  this->show();

  m_notifyMgr->setTooltip("Quran Companion");
  if (m_settings->value("VOTD").toBool())
    m_notifyMgr->checkDailyVerse();

  m_popup->dockLocationChanged(dockWidgetArea(ui->sideDock));
}

/* ------------------------ Initalization methods ------------------------ */

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

/*!
 * \brief MainWindow::init initalizes different parts used by the app, such as
 * the quran page widget, db manager, and the verse player objects
 */
void
MainWindow::init()
{
  // initalization
  m_dbMgr = new DBManager(this);
  m_player = new VersePlayer(
    this, m_dbMgr, m_currVerse, m_settings->value("Reciter", 0).toInt());
  m_quranBrowser =
    new QuranPageBrowser(ui->frmPageContent, m_dbMgr, m_currVerse.page);
  m_popup = new NotificationPopup(this, m_dbMgr);

  ui->frmPageContent->layout()->addWidget(m_quranBrowser);

  ui->cmbPage->setValidator(new QIntValidator(1, 604, this));
  m_notifyMgr = new NotificationManager(this, m_dbMgr);

  updateLoadedTafsir();
  updateLoadedTranslation();
  updateSideFont();

  redrawQuranPage(true);
  setVerseComboBoxRange(true);

  QVBoxLayout* vbl = new QVBoxLayout();
  vbl->setDirection(QBoxLayout::BottomToTop);
  ui->scrlVerseCont->setLayout(vbl);
  addSideContent();

  for (int i = 1; i < 605; i++) {
    ui->cmbPage->addItem(QString::number(i));
  }

  foreach (Reciter r, m_recitersList) {
    ui->cmbReciter->addItem(r.displayName);
  }

  // sets without emitting signal
  setCmbVerseIdx(m_currVerse.number - 1);
  setCmbJozzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);

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

/*!
 * \brief MainWindow::setupConnections connects different UI components with
 * signals and slots
 */
void
MainWindow::setupConnections()
{
  QShortcut* spaceKey = new QShortcut(Qt::Key_Space, this);
  spaceKey->setContext(Qt::ApplicationShortcut);

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
          &MainWindow::openSearchDialog,
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
  connect(m_quranBrowser,
          &QTextBrowser::anchorClicked,
          this,
          &MainWindow::verseAnchorClicked);
  connect(m_quranBrowser,
          &QuranPageBrowser::copyVerse,
          this,
          &MainWindow::copyVerseText);

  // ########## page controls ########## //
  connect(ui->btnNext,
          &QPushButton::clicked,
          this,
          &MainWindow::nextPage,
          Qt::UniqueConnection);
  connect(ui->btnPrev,
          &QPushButton::clicked,
          this,
          &MainWindow::prevPage,
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
          &MainWindow::cmbJozzChanged,
          Qt::UniqueConnection);
  connect(m_player,
          &VersePlayer::surahChanged,
          this,
          &MainWindow::updateSurah,
          Qt::UniqueConnection);
  connect(m_player,
          &VersePlayer::verseNoChanged,
          this,
          &MainWindow::activeVerseChanged,
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
  connect(ui->sideDock,
          &QDockWidget::dockLocationChanged,
          m_popup,
          &NotificationPopup::dockLocationChanged,
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
  connect(spaceKey,
          &QShortcut::activated,
          this,
          &MainWindow::spaceKeyPressed,
          Qt::UniqueConnection);

  // ########## system tray ########## //
  connect(m_player,
          &QMediaPlayer::playbackStateChanged,
          this,
          &MainWindow::updateTrayTooltip,
          Qt::UniqueConnection);
  connect(m_notifyMgr, &NotificationManager::exit, this, &QApplication::exit);
  connect(m_notifyMgr,
          &NotificationManager::togglePlayback,
          this,
          &MainWindow::spaceKeyPressed,
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
}

/* ------------------------ Help menu actions ------------------------ */

void
MainWindow::checkForUpdates()
{
  QFileInfo tool(m_updateToolPath);
  if (tool.exists()) {
    m_process->setWorkingDirectory(QApplication::applicationDirPath());
    m_process->start(m_updateToolPath, QStringList("ch"));
  } else {
    QMessageBox::warning(this, tr("Error"), tr("Update tool is unavailable."));
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

/* ------------------------ UI updating ------------------------ */

void
MainWindow::updateSurah()
{
  QItemSelectionModel* select = ui->listViewSurahs->selectionModel();
  select->clearSelection();
  QModelIndex surah = m_surahListModel.index(m_player->activeVerse().surah - 1);
  select->select(surah,
                 QItemSelectionModel::Rows | QItemSelectionModel::Select);
  ui->listViewSurahs->scrollTo(surah, QAbstractItemView::PositionAtCenter);
  if (m_player->activeVerse().surah != m_currVerse.surah) {
    surahClicked(surah);
  }
}

/*!
 * \brief MainWindow::updatePageVerseInfoList updates the list that contains all
 * verses in the current page each as a QMap of keys "surah" & "ayah"
 */
void
MainWindow::updatePageVerseInfoList()
{
  m_vInfoList = m_dbMgr->getVerseInfoList(m_currVerse.page);
}

void
MainWindow::setVerseToStartOfPage()
{
  // set the current verse to the verse at the top of the page
  m_currVerse = m_vInfoList.at(0);

  if (m_player->activeVerse() != m_currVerse) {
    // update the player active verse
    m_player->setVerse(m_currVerse);
    // open newly set verse recitation file
    m_player->setVerseFile(m_player->constructVerseFilename());
  }
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
  m_internalVerseChange = true;
  ui->cmbVerse->setCurrentIndex(idx);
  m_internalVerseChange = false;
}

void
MainWindow::setCmbJozzIdx(int idx)
{
  m_internalJozzChange = true;
  ui->cmbJuz->setCurrentIndex(idx);
  m_internalJozzChange = false;
}

/*!
 * \brief MainWindow::setVerseComboBoxRange sets the verse combobox values
 * according to the current surah verse count, sets the current verse visible
 *
 * @param forceUpdate flag forces the combobox to update it's surah verse count
 */
void
MainWindow::setVerseComboBoxRange(bool forceUpdate)
{
  int oldCount = m_player->surahCount();
  m_player->updateSurahVerseCount();
  int updatedCount = m_player->surahCount();

  if (updatedCount != oldCount || forceUpdate) {
    if (m_verseValidator != nullptr)
      delete m_verseValidator;
    m_verseValidator = new QIntValidator(1, updatedCount, ui->cmbVerse);

    // updates values in the combobox with the current surah verses
    ui->cmbVerse->clear();
    m_internalVerseChange = true;
    for (int i = 1; i <= updatedCount; i++)
      ui->cmbVerse->addItem(QString::number(i), i);
    m_internalVerseChange = false;

    ui->cmbVerse->setValidator(m_verseValidator);
  }

  setCmbVerseIdx(m_currVerse.number - 1);
}

/* ------------------------ Page navigation ------------------------ */

/*!
 * \brief MainWindow::gotoPage displays the given page and sets the current
 * verse to the 1st verse in the page
 *
 * @param page to navigate to
 */
void
MainWindow::gotoPage(int page, bool automaticFlip)
{
  m_currVerse.page = page;
  redrawQuranPage();

  if (!automaticFlip)
    btnStopClicked();
  else {
    m_endOfPage = false;
    setVerseToStartOfPage();
    updateSurah();
    setVerseComboBoxRange();
  }

  setCmbJozzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);
  addSideContent();
}

/*!
 * \brief MainWindow::nextPage navigates to the next page relative to the
 * current page
 */
void
MainWindow::nextPage()
{
  bool keepPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
  if (m_currVerse.page < 604) {
    setCmbPageIdx(m_currVerse.page);
    gotoPage(m_currVerse.page + 1, true);

    ui->scrlVerseByVerse->verticalScrollBar()->setValue(0);
    // if the page is flipped automatically, resume playback
    if (keepPlaying) {
      m_player->play();
      highlightCurrentVerse();
    }
  }
}

/*!
 * \brief MainWindow::prevPage navigates to the previous page relative to the
 * current page
 */
void
MainWindow::prevPage()
{
  bool keepPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
  if (m_currVerse.page > 1) {
    setCmbPageIdx(m_currVerse.page - 2);
    gotoPage(m_currVerse.page - 1, true);

    ui->scrlVerseByVerse->verticalScrollBar()->setValue(0);
    if (keepPlaying) {
      m_player->play();
      highlightCurrentVerse();
    }
  }
}

/*!
 * \brief MainWindow::gotoSurah gets the page of the 1st verse in this surah,
 * moves to that page, and starts playback of the surah.
 *
 * @param surahIdx surah
 * number in the mushaf (1-114)
 */
void
MainWindow::gotoSurah(int surahIdx)
{
  // getting surah index
  m_currVerse.page = m_dbMgr->getSurahStartPage(surahIdx);
  m_currVerse.surah = surahIdx;
  m_currVerse.number = 1;

  // setting up the page of verse 1
  redrawQuranPage();
  addSideContent();

  // syncing the player & playing basmalah
  m_player->setVerse(m_currVerse);
  m_player->playBasmalah();
  highlightCurrentVerse();

  setCmbPageIdx(m_currVerse.page - 1);
  setCmbJozzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);
  setVerseComboBoxRange();

  m_endOfPage = false;
}

void
MainWindow::searchSurahTextChanged(const QString& arg1)
{
  if (arg1.isEmpty()) {
    m_surahListModel.setStringList(m_surahList);
    updateSurah();
  } else {
    QList<int> suggestions = m_dbMgr->searchSurahNames(arg1);
    QStringList res;
    for (int i = 0; i < 114; i++) {
      if (suggestions.contains(i + 1)) {
        res.append(m_surahList.at(i));
      }
    }

    m_surahListModel.setStringList(res);
  }
}

void
MainWindow::listSurahNameClicked(const QModelIndex& index)
{
  int s = 0;
  for (int i = 0; i < 114; i++) {
    if (m_surahList.at(i) == index.data().toString()) {
      s = i + 1;
      break;
    }
  }
  gotoSurah(s);
}

/*!
 * \brief MainWindow::cmbPageChanged slot for updating the reader page as the
 * user selects a different page from the combobox.
 *
 * @param newIdx the selected idx in the combobox (0-603)
 */
void
MainWindow::cmbPageChanged(int newIdx)
{
  if (m_internalPageChange) {
    qDebug() << "Internal page change";
    return;
  }

  gotoPage(newIdx + 1);
}

/*!
 * \brief MainWindow::cmbVerseChanged slot for updating the reader page as the
 * user selects a different verse in the same surah.
 *
 * @param newVerseIdx verse idx in the combobox (0 -> (surahVerseCount-1))
 */
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
  m_currVerse.page = m_dbMgr->getVersePage(m_currVerse.surah, verse);
  m_currVerse.number = verse;

  redrawQuranPage();

  // update the player surah & verse
  m_player->setVerse(m_currVerse);
  // open newly set verse recitation file
  m_player->setVerseFile(m_player->constructVerseFilename());

  setCmbPageIdx(m_currVerse.page - 1);
  setCmbJozzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);

  addSideContent();
  m_endOfPage = false;
}

void
MainWindow::cmbJozzChanged(int newJozzIdx)
{
  if (m_internalJozzChange) {
    qDebug() << "Internal jozz change";
    return;
  }

  gotoPage(m_dbMgr->getJuzStartPage(newJozzIdx + 1));
}

/* ------------------------ Player controls / highlighting
 * ------------------------ */

void
MainWindow::spaceKeyPressed()
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

/*!
 * \brief MainWindow::btnPlayClicked continues playback of the current verse
 */
void
MainWindow::btnPlayClicked()
{
  // If now playing the last verse in the page, set the flag to flip the page
  if (m_currVerse.number == m_vInfoList.last().number &&
      m_currVerse.number != m_player->surahCount()) {
    m_endOfPage = true;
  }

  highlightCurrentVerse();
  m_player->play();
}

/*!
 * \brief MainWindow::btnStopClicked stops playback, sets the current vers to
 * the 1st in the page, updates comboboxes as it might be a different surah
 */
void
MainWindow::btnStopClicked()
{
  m_player->stop();
  setVerseToStartOfPage();

  m_internalSurahChange = true;
  updateSurah();
  setVerseComboBoxRange();
  m_internalSurahChange = false;

  m_endOfPage = false;
}

/*!
 * \brief MainWindow::mediaStateChanged disables/enables control buttons
 * according to the media player state.
 *
 * @param state
 */
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
}

/*!
 * \brief MainWindow::mediaPosChanged sets the current position in the audio
 * file as the position of the slider.
 *
 * @param position position in audio file in milliseconds
 */
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

/*!
 * \brief MainWindow::missingRecitationFileWarn display warning message box in
 * case that recitation files are missing
 */
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
    if (m_downloaderDlg != nullptr)
      m_downloaderDlg->selectTask(reciterIdx, surah);
  }
}

/*!
 * \brief MainWindow::activeVerseChanged sync the main window with the verse
 * player as active verse changes, set the endOfPage flag or flip page if the
 * flag is set
 */
void
MainWindow::activeVerseChanged()
{
  m_currVerse = { m_currVerse.page,
                  m_player->activeVerse().surah,
                  m_player->activeVerse().number };

  if (m_currVerse.number == 0)
    m_currVerse.number = 1;

  setCmbVerseIdx(m_currVerse.number - 1);
  if (m_endOfPage) {
    m_endOfPage = false;
    nextPage();
  }
  // If now playing the last verse in the page, set the flag to flip the page
  if (m_currVerse.number == m_vInfoList.last().number &&
      m_currVerse.number != m_player->surahCount()) {
    m_endOfPage = true;
  }

  highlightCurrentVerse();
}

/*!
 * \brief MainWindow::verseClicked slot to navigate to the clicked verse in the
 * side panel, sync player, and copy aya text to clipboard
 */
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
    updateSurah();
  }

  setCmbVerseIdx(verse - 1);
  m_endOfPage = false;
  m_player->setVerseFile(m_player->constructVerseFilename());
  btnPlayClicked();
}

void
MainWindow::surahClicked(QModelIndex& index)
{
  int s = index.row() + 1;
  gotoSurah(s);
}

/*!
 * \brief MainWindow::verseAnchorClicked is the callback function for clicking
 * verses in the QuranPageBrowser that determines which action to take based on
 * the chosen option in the menu.
 *
 * @param hrefUrl "#idx" where idx is the verse
 * index in the page/pageVerseInfo list
 */
void
MainWindow::verseAnchorClicked(const QUrl& hrefUrl)
{
  QString idx = hrefUrl.toString();
  idx.remove('#');
  Verse v = m_vInfoList.at(idx.toInt());

  int chosenAction = m_quranBrowser->lmbVerseMenu(m_dbMgr->isBookmarked(v));
  // remove from / add to favorites
  if (chosenAction == 5) {
    if (m_dbMgr->removeBookmark(v))
      m_popup->bookmarkRemoved();

  } else if (chosenAction == 4) {
    m_dbMgr->addBookmark(v);
    m_popup->bookmarkAdded();
  }
  // copy
  else if (chosenAction == 3) {
    copyVerseText(idx.toInt());
    m_popup->copiedToClipboard();
  }
  // open tafsir
  else if (chosenAction == 2)
    showExpandedVerseTafsir(v);
  // select or play
  else if (chosenAction == 0 || chosenAction == 1) {
    m_currVerse.number = v.number;
    m_player->setVerse(m_currVerse);

    if (m_currVerse.surah != v.surah) {
      m_currVerse.surah = v.surah;

      m_player->setVerse(m_currVerse);
      m_player->updateSurahVerseCount();
      setVerseComboBoxRange();
      updateSurah();
    }

    m_internalVerseChange = true;
    ui->cmbVerse->setCurrentIndex(v.number - 1);

    m_internalVerseChange = false;

    m_endOfPage = false;
    switch (chosenAction) {
      case 0:
        m_player->setVerseFile(m_player->constructVerseFilename());
        btnPlayClicked();
        break;
      case 1:
        m_player->setSource(QUrl());
        highlightCurrentVerse();
        break;
      default:
        break;
    }
  }
}

/* ------------------------ Preferences/Downloader dialog methods
 * ------------------------ */

/*!
 * \brief MainWindow::actionPrefTriggered open the settings dialog and connect
 * settings change slots
 */
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
            m_quranBrowser,
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
  }

  m_settingsDlg->showWindow();
}

/*!
 * \brief MainWindow::actionDMTriggered open the download manager dialog, create
 * downloadmanager instance if it's not set
 */
void
MainWindow::actionDMTriggered()
{
  if (m_downloaderDlg == nullptr) {
    if (m_downManPtr == nullptr) {
      m_downManPtr = new DownloadManager(this, m_dbMgr);
      connect(m_downManPtr,
              &DownloadManager::downloadComplete,
              m_popup,
              &NotificationPopup::completedDownload,
              Qt::UniqueConnection);
    }

    m_downloaderDlg = new DownloaderDialog(this, m_downManPtr, m_dbMgr);
  }

  m_downloaderDlg->show();
}

void
MainWindow::actionBookmarksTriggered()
{
  if (m_bookmarksDlg == nullptr) {
    m_bookmarksDlg = new BookmarksDialog(this, m_dbMgr);
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
  showExpandedVerseTafsir(m_currVerse);
}

void
MainWindow::actionVotdTriggered()
{
  showVOTDmessage(m_notifyMgr->votd());
}

void
MainWindow::actionAboutTriggered()
{
  QString about =
    "<h2>" + tr("Quran Companion v") + qApp->applicationVersion() +
    +"</h2><p><a href='https://github.com/0xzer0x/quran-companion'>" +
    tr("Quran Companion") + "</a>" +
    tr(" is a free cross-platform Quran reader & player.") + "</p><p>" +
    tr("Licensed under the ") +
    "<a href='https://ojuba.org/"
    "waqf-2.0:%D8%B1%D8%AE%D8%B5%D8%A9_%D9%88%D9%82%D9%81_%D8%A7%D9%84%D8%"
    "B9%D8%A7%D9%85%D8%A9'>" +
    tr("'Waqf' General Public License") + "</a>.</p>";

  QMessageBox::about(this, tr("About Quran Companion"), about);
}

void
MainWindow::on_actionAbout_Qt_triggered()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

/* ------------------------ Search dialog ------------------------ */

/*!
 * \brief MainWindow::openSearchDialog open the verse search dialog
 */
void
MainWindow::openSearchDialog()
{
  if (m_searchDlg == nullptr) {
    m_searchDlg = new SearchDialog(this, m_dbMgr);
    connect(m_searchDlg,
            &SearchDialog::navigateToVerse,
            this,
            &MainWindow::navigateToVerse);
  }

  m_searchDlg->show();
}

/*!
 * \brief MainWindow::navigateToVerse navigate to a selected verse from the
 * search results
 *
 * @param v Verse to navigate to
 */
void
MainWindow::navigateToVerse(Verse v)
{
  m_currVerse = v;

  redrawQuranPage();
  addSideContent();

  m_player->setVerse(m_currVerse);
  setVerseComboBoxRange();

  setCmbPageIdx(m_currVerse.page - 1);
  setCmbVerseIdx(m_currVerse.number - 1);
  setCmbJozzIdx(m_dbMgr->getJuzOfPage(m_currVerse.page) - 1);

  updateSurah();
  highlightCurrentVerse();

  m_player->setVerseFile(m_player->constructVerseFilename());
  m_endOfPage = false;
}

/* ------------------------ Settings update methods ------------------------ */

/*!
 * \brief MainWindow::updateLoadedTafsir set tafsir to the one in the settings,
 * update the selected db
 */
void
MainWindow::updateLoadedTafsir()
{
  DBManager::Tafsir currTafsir =
    qvariant_cast<DBManager::Tafsir>(m_settings->value("Reader/Tafsir"));

  m_dbMgr->setCurrentTafsir(currTafsir);
}

/*!
 * \brief MainWindow::updateLoadedTranslation set translation to the one in the
 * settings, update the selected db
 */
void
MainWindow::updateLoadedTranslation()
{
  DBManager::Translation currTrans = qvariant_cast<DBManager::Translation>(
    m_settings->value("Reader/Translation"));

  m_dbMgr->setCurrentTranslation(currTrans);
}

/*!
 * \brief MainWindow::updateSideFont set side content font to the one in the
 * settings
 */
void
MainWindow::updateSideFont()
{
  m_sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
}

/* ------------------------ Content generation ------------------------ */

/*!
 * \brief MainWindow::redrawQuranPage redraw the current quran page
 */
void
MainWindow::redrawQuranPage(bool manualSz)
{
  m_quranBrowser->constructPage(m_currVerse.page, manualSz);
  updatePageVerseInfoList();
}

/*!
 * \brief MainWindow::highlightCurrentVerse highlights the currently
 * selected/recited verse in the quran page & side panel
 */
void
MainWindow::highlightCurrentVerse()
{
  int idx;
  for (idx = 0; idx < m_vInfoList.size(); idx++) {
    Verse v = m_vInfoList.at(idx);
    if (m_currVerse == v)
      break;
  }
  m_quranBrowser->highlightVerse(idx);

  if (m_highlightedFrm != nullptr)
    m_highlightedFrm->setStyleSheet("");

  HighlightFrame* verseFrame =
    ui->scrlVerseCont->findChild<HighlightFrame*>(QString("%0_%1").arg(
      QString::number(m_currVerse.surah), QString::number(m_currVerse.number)));

  verseFrame->highlightFrame();

  if (m_highlightedFrm != nullptr) {
    ui->scrlVerseByVerse->ensureWidgetVisible(verseFrame);
  }

  m_highlightedFrm = verseFrame;
}

/*!
 * \brief MainWindow::addSideContent updates the side panel with the chosen side
 * content type
 */
void
MainWindow::addSideContent()
{
  if (!m_verseFrameList.isEmpty()) {
    qDeleteAll(m_verseFrameList);
    m_verseFrameList.clear();
    m_highlightedFrm = nullptr;
  }

  ClickableLabel* verselb;
  QLabel* contentLb;
  HighlightFrame* verseContFrame;
  QString prevLbContent, currLbContent;
  for (int i = m_vInfoList.size() - 1; i >= 0; i--) {
    Verse vInfo = m_vInfoList.at(i);

    verseContFrame = new HighlightFrame(ui->scrlVerseCont);
    verselb = new ClickableLabel(verseContFrame);
    contentLb = new QLabel(verseContFrame);

    verseContFrame->setObjectName(
      QString("%0_%1").arg(vInfo.surah).arg(vInfo.number));

    verselb->setFont(
      QFont(m_quranBrowser->pageFont(), m_quranBrowser->fontSize() - 2));
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
    ui->scrlVerseCont->layout()->addWidget(verseContFrame);
    m_verseFrameList.insert(0, verseContFrame);

    // connect clicked signal for each label
    connect(verselb,
            &ClickableLabel::clicked,
            this,
            &MainWindow::verseClicked,
            Qt::UniqueConnection);
  }

  if (m_player->playbackState() == QMediaPlayer::PlayingState) {
    highlightCurrentVerse();
  }
}

void
MainWindow::updateTrayTooltip(QMediaPlayer::PlaybackState newState)
{
  if (newState == QMediaPlayer::PlayingState) {
    m_notifyMgr->setTooltip(tr("Now playing: ") + m_player->reciterName() +
                            " - " + tr("Surah ") +
                            m_dbMgr->getSurahName(m_currVerse.surah));
  } else
    m_notifyMgr->setTooltip("Quran Companion");
}

/*!
 * \brief MainWindow::showExpandedVerseTafsir toggle a collapsed verse tafsir
 */
void
MainWindow::showExpandedVerseTafsir(Verse v)
{
  if (m_tafsirDlg == nullptr) {
    m_tafsirDlg = new TafsirDialog(this, m_dbMgr);
  }

  m_tafsirDlg->setShownVerse(v);
  m_tafsirDlg->loadVerseTafsir();
  m_tafsirDlg->show();
}

/* ------------------------ Helper functions ------------------------ */

void
MainWindow::copyVerseText(int IdxInPage)
{
  const Verse& v = m_vInfoList.at(IdxInPage);
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
  m_settings->setValue("Verse", m_currVerse.number);
  m_settings->endGroup();

  m_settings->sync();
}

void
MainWindow::restartApp()
{
  saveReaderState();
  emit QApplication::exit();
  QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
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
