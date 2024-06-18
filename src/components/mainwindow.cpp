/**
 * @file mainwindow.cpp
 * @brief Implementation file for MainWindow
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtAwesome.h>
#include <dialogs/aboutdialog.h>
#include <dialogs/khatmahdialog.h>
#include <player/impl/setplaybackstrategy.h>
#include <player/playbackcontroller.h>
#include <utils/servicefactory.h>
#include <utils/stylemanager.h>
using namespace fa;
using std::make_pair;

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_verseValidator(new QIntValidator(this))
  , m_currVerse(Verse::getCurrent())
  , m_navigator(Navigator::getInstance())
  , m_config(Configuration::getInstance())
  , m_shortcutHandler(ShortcutHandler::getInstance())
  , m_bookmarkService(ServiceFactory::bookmarkService())
  , m_quranService(ServiceFactory::quranService())
  , m_khatmahService(ServiceFactory::khatmahService())
  , m_translationService(ServiceFactory::translationService())
  , m_reciters(Reciter::reciters)
  , m_tafasir(Tafsir::tafasir)
{
  ui->setupUi(this);
  loadIcons();
  loadVerse();
  loadComponents();

  if (m_config.settings().value("WindowState").isNull())
    m_config.settings().setValue("WindowState", saveState());
  else
    restoreState(m_config.settings().value("WindowState").toByteArray());

  // connectors
  setupShortcuts();
  setupConnections();
  setupSurahsDock();
  setupMenubarButton();
  this->show();

  m_popup->setDockArea(dockWidgetArea(ui->sideDock));
}

void
MainWindow::loadIcons()
{
  fa::QtAwesome& awesome = StyleManager::getInstance().awesome();
  ui->actionKhatmah->setIcon(awesome.icon(fa_solid, fa_list));
  ui->actionDownloadManager->setIcon(awesome.icon(fa_solid, fa_download));
  ui->actionExit->setIcon(awesome.icon(fa_solid, fa_xmark));
  ui->actionFind->setIcon(awesome.icon(fa_solid, fa_magnifying_glass));
  ui->actionTafsir->setIcon(awesome.icon(fa_solid, fa_book_open));
  ui->actionVOTD->setIcon(awesome.icon(fa_solid, fa_calendar_day));
  ui->actionBookmarks->setIcon(awesome.icon(fa_solid, fa_bookmark));
  ui->actionPereferences->setIcon(awesome.icon(fa_solid, fa_gear));
  ui->actionAdvancedCopy->setIcon(awesome.icon(fa_solid, fa_clipboard));
  ui->actionReaderViewToggle->setIcon(awesome.icon(fa_solid, fa_columns));
  ui->actionUpdates->setIcon(awesome.icon(fa_solid, fa_arrow_rotate_right));
  ui->actionImport->setIcon(awesome.icon(fa_solid, fa_file_arrow_down));
  ui->actionExport->setIcon(awesome.icon(fa_solid, fa_file_arrow_up));
}

void
MainWindow::loadVerse()
{
  int id = m_config.settings().value("Reader/Khatmah").toInt();
  m_khatmahService->setActiveKhatmah(id);

  std::optional<Verse> khatmahPos = m_khatmahService->loadVerse(id);

  if (!khatmahPos.has_value()) {
    QString name = id ? QString::number(id) : tr("Default");
    m_khatmahService->addKhatmah(Verse(1, 1, 1), name, id);
  }

  m_currVerse.update(khatmahPos.value_or(Verse(1, 1, 1)));
}

void
MainWindow::loadComponents()
{
  QPointer<VersePlayer> player =
    new VersePlayer(this, m_config.settings().value("Reciter", 0).toInt());
  m_playbackController = new PlaybackController(this, player);
  m_reader = new QuranReader(this, m_playbackController);
  m_playerControls = new PlayerControls(this, m_playbackController, m_reader);
  m_settingsDlg = new SettingsDialog(this, player);
  m_popup = new NotificationPopup(this);
  m_betaqaViewer = new BetaqaViewer(this);
  m_verseDlg = new VerseDialog(this);
  m_cpyDlg = new CopyDialog(this);
  m_systemTray = new SystemTray(this);
  m_contentDlg = new ContentDialog(this);
  m_jobMgr = new JobManager(this);
  m_versionChecker = new VersionChecker(this);
  m_selectorDlg = new FileSelector(this);
  m_importExportDlg =
    new ImportExportDialog(this,
                           QSharedPointer<JsonDataImporter>::create(),
                           QSharedPointer<JsonDataExporter>::create());

  QHBoxLayout* controls = new QHBoxLayout();
  QFrame* controlsFrame = new QFrame(this);
  controls->setAlignment(Qt::AlignCenter);
  controls->setContentsMargins(0, 0, 0, 0);
  controls->setSpacing(0);
  controls->addStretch(1);
  controls->addWidget(m_playerControls);
  controls->addStretch(1);
  controlsFrame->setLayout(controls);
  ui->scrollAreaWidgetContents->layout()->addWidget(controlsFrame);
  ui->scrollAreaWidgetContents->layout()->addWidget(m_reader);

  ui->cmbPage->setValidator(new QIntValidator(1, 604, this));
  ui->cmbJuz->setValidator(new QIntValidator(1, 30, this));

  setVerseComboBoxRange(true);

  for (int i = 1; i < 605; i++) {
    ui->cmbPage->addItem(QString::number(i));
  }

  // sets without emitting signal
  setCmbVerseIdx(m_currVerse.number() - 1);
  setCmbJuzIdx(m_quranService->getVerseJuz(m_currVerse) - 1);

  ui->cmbPage->setCurrentIndex(m_currVerse.page() - 1);
}

void
MainWindow::setupConnections()
{
  connectMenubar();
  connectTray();
  connectReader();
  connectPlayer();
  connectControls();
  connectSettings();
  connectNotifiers();
  m_navigator.addObserver(this);
}

void
MainWindow::setupShortcuts()
{
  m_shortcutHandler.createShortcuts(this);
  connect(
    &m_shortcutHandler, &ShortcutHandler::togglePlayerControls, this, [this]() {
      ui->actionPlayerControls->setChecked(
        !ui->actionPlayerControls->isChecked());
    });

  for (const auto& connection : {
         make_pair(&ShortcutHandler::toggleMenubar, &MainWindow::toggleMenubar),
         make_pair(&ShortcutHandler::toggleNavDock, &MainWindow::toggleNavDock),
         make_pair(&ShortcutHandler::nextVerse, &MainWindow::nextVerse),
         make_pair(&ShortcutHandler::prevVerse, &MainWindow::prevVerse),
         make_pair(&ShortcutHandler::nextSurah, &MainWindow::nextSurah),
         make_pair(&ShortcutHandler::prevSurah, &MainWindow::prevSurah),
         make_pair(&ShortcutHandler::nextJuz, &MainWindow::nextJuz),
         make_pair(&ShortcutHandler::prevJuz, &MainWindow::prevJuz),
         make_pair(&ShortcutHandler::bookmarkCurrent,
                   &MainWindow::bookmarkCurrent),
         make_pair(&ShortcutHandler::openDownloads,
                   &MainWindow::actionDMTriggered),
         make_pair(&ShortcutHandler::openBookmarks,
                   &MainWindow::actionBookmarksTriggered),
         make_pair(&ShortcutHandler::openKhatmah,
                   &MainWindow::actionKhatmahTriggered),
         make_pair(&ShortcutHandler::openSearch,
                   &MainWindow::actionSearchTriggered),
         make_pair(&ShortcutHandler::openSettings,
                   &MainWindow::actionPrefTriggered),
         make_pair(&ShortcutHandler::openContent,
                   &MainWindow::actionTafsirTriggered),
         make_pair(&ShortcutHandler::openAdvancedCopy,
                   &MainWindow::actionAdvancedCopyTriggered),
       }) {
    connect(&m_shortcutHandler, connection.first, this, connection.second);
  }
}

void
MainWindow::connectPlayer()
{
  connect(m_playbackController->player(),
          &VersePlayer::missingVerseFile,
          this,
          &MainWindow::missingRecitationFileWarn);
  connect(m_playbackController->player(),
          &QMediaPlayer::playbackStateChanged,
          this,
          &MainWindow::updateTrayTooltip);
}

void
MainWindow::connectTray()
{
  connect(m_systemTray, &SystemTray::exit, this, &QApplication::exit);
  connect(m_systemTray,
          &SystemTray::togglePlayback,
          m_playerControls,
          &PlayerControls::togglePlayback);
  connect(m_systemTray, &SystemTray::showWindow, this, &MainWindow::show);
  connect(m_systemTray, &SystemTray::hideWindow, this, &MainWindow::hide);
  connect(m_systemTray,
          &SystemTray::checkForUpdates,
          m_versionChecker,
          &VersionChecker::checkUpdates);
  connect(m_systemTray,
          &SystemTray::openAbout,
          this,
          &MainWindow::actionAboutTriggered);
  connect(m_systemTray,
          &SystemTray::openPrefs,
          this,
          &MainWindow::actionPrefTriggered);
}

void
MainWindow::connectReader()
{
  connect(m_reader,
          &QuranReader::copyVerseText,
          m_cpyDlg,
          &CopyDialog::copyVerseText);
  connect(m_reader,
          &QuranReader::showVerseTafsir,
          m_contentDlg,
          &ContentDialog::showVerseTafsir);
  connect(m_reader,
          &QuranReader::showVerseTranslation,
          m_contentDlg,
          &ContentDialog::showVerseTranslation);
  connect(m_reader,
          &QuranReader::showVerseThoughts,
          m_contentDlg,
          &ContentDialog::showVerseThoughts);
  connect(m_reader,
          &QuranReader::showBetaqa,
          m_betaqaViewer,
          &BetaqaViewer::showSurah);
  connect(m_contentDlg,
          &ContentDialog::missingTafsir,
          this,
          &MainWindow::missingTafsir);
  connect(m_contentDlg,
          &ContentDialog::missingTranslation,
          this,
          &MainWindow::missingTranslation);
}

void
MainWindow::connectControls()
{
  // ########## page controls ########## //
  connect(ui->cmbPage,
          &QComboBox::currentIndexChanged,
          this,
          &MainWindow::cmbPageChanged);
  connect(ui->cmbVerse,
          &QComboBox::currentIndexChanged,
          this,
          &MainWindow::cmbVerseChanged);
  connect(ui->cmbJuz,
          &QComboBox::currentIndexChanged,
          this,
          &MainWindow::cmbJuzChanged);
  // ########## navigation dock ########## //
  connect(ui->lineEditSearchSurah,
          &QLineEdit::textChanged,
          this,
          &MainWindow::searchSurahTextChanged);
  connect(ui->listViewSurahs,
          &QListView::clicked,
          this,
          &MainWindow::listSurahNameClicked);
}

void
MainWindow::connectSettings()
{
  connect(
    m_settingsDlg, &SettingsDialog::restartApp, this, &MainWindow::restartApp);
  // qcf2 missing files warning
  connect(
    m_settingsDlg, &SettingsDialog::qcf2Missing, this, &MainWindow::missingQCF);
  // Quran page signals
  connect(m_settingsDlg,
          &SettingsDialog::redrawQuranPage,
          m_reader,
          &QuranReader::redrawQuranPage);
  connect(m_settingsDlg,
          &SettingsDialog::highlightLayerChanged,
          m_reader,
          &QuranReader::updateHighlight);
  connect(m_settingsDlg,
          &SettingsDialog::quranFontChanged,
          m_reader,
          &QuranReader::updatePageFontSize);
  // Side panel signals
  connect(m_settingsDlg,
          &SettingsDialog::redrawSideContent,
          m_reader,
          &QuranReader::addSideContent);
  connect(m_settingsDlg,
          &SettingsDialog::translationChanged,
          m_translationService,
          &TranslationService::loadTranslation);
  connect(m_settingsDlg,
          &SettingsDialog::sideFontChanged,
          m_reader,
          &QuranReader::updateSideFont);
  connect(m_settingsDlg,
          &SettingsDialog::verseTypeChanged,
          m_reader,
          &QuranReader::updateVerseType);
  // audio device signals
  connect(m_settingsDlg,
          &SettingsDialog::usedAudioDeviceChanged,
          m_playbackController->player(),
          &VersePlayer::changeUsedAudioDevice);
  // shortcut change
  connect(m_settingsDlg,
          &SettingsDialog::shortcutChanged,
          &m_shortcutHandler,
          &ShortcutHandler::shortcutChanged);
}

void
MainWindow::connectMenubar()
{
  connect(ui->actionExit, &QAction::triggered, this, &QApplication::exit);
  connect(ui->actionReaderViewToggle,
          &QAction::triggered,
          m_reader,
          &QuranReader::toggleReaderView);
  connect(ui->actionPlayerControls,
          &QAction::toggled,
          this,
          &MainWindow::actionPlayerControlsToggled);
  for (const auto& connection : {
         make_pair(ui->actionPereferences, &MainWindow::actionPrefTriggered),
         make_pair(ui->actionDownloadManager, &MainWindow::actionDMTriggered),
         make_pair(ui->actionFind, &MainWindow::actionSearchTriggered),
         make_pair(ui->actionTafsir, &MainWindow::actionTafsirTriggered),
         make_pair(ui->actionVOTD, &MainWindow::actionVotdTriggered),
         make_pair(ui->actionBookmarks, &MainWindow::actionBookmarksTriggered),
         make_pair(ui->actionKhatmah, &MainWindow::actionKhatmahTriggered),
         make_pair(ui->actionAboutQC, &MainWindow::actionAboutTriggered),
         make_pair(ui->actionAboutQt, &MainWindow::actionAboutQttriggered),
         make_pair(ui->actionUpdates, &MainWindow::actionUpdatesTriggered),
         make_pair(ui->actionImport, &MainWindow::importUserData),
         make_pair(ui->actionExport, &MainWindow::exportUserData),
         make_pair(ui->actionAdvancedCopy,
                   &MainWindow::actionAdvancedCopyTriggered),
       }) {
    connect(connection.first, &QAction::triggered, this, connection.second);
  }
}

void
MainWindow::connectNotifiers()
{
  m_popup->registerSender(m_jobMgr->notifier());
  m_popup->registerSender(m_versionChecker->notifier());
  m_popup->registerSender(m_cpyDlg->notifier());
  m_popup->registerSender(m_bookmarkService->notifier());
  connect(ui->sideDock,
          &QDockWidget::dockLocationChanged,
          m_popup,
          &NotificationPopup::setDockArea);
}

void
MainWindow::setupSurahsDock()
{
  for (int i = 1; i < 115; i++) {
    QString item = QString::number(i).rightJustified(3, '0') + ' ' +
                   m_quranService->surahNames().at(i - 1);
    m_surahList.append(item);
  }

  m_surahListModel.setStringList(m_surahList);
  ui->listViewSurahs->setModel(&m_surahListModel);

  QItemSelectionModel* selector = ui->listViewSurahs->selectionModel();
  selector->select(m_surahListModel.index(m_currVerse.surah() - 1),
                   QItemSelectionModel::Rows | QItemSelectionModel::Select);

  ui->listViewSurahs->scrollTo(m_surahListModel.index(m_currVerse.surah() - 1),
                               QAbstractItemView::PositionAtCenter);
}

void
MainWindow::setupMenubarButton()
{
  QPushButton* toggleNav = new QPushButton(this);
  toggleNav->setObjectName("btnToggleNav");
  toggleNav->setCheckable(true);
  toggleNav->setChecked(!ui->sideDock->isHidden());
  toggleNav->setCursor(Qt::PointingHandCursor);
  toggleNav->setToolTip(tr("Navigation"));
  toggleNav->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  toggleNav->setStyleSheet(
    QString("QPushButton { min-height: %0px; min-width: %1px; }")
      .arg(QString::number(ui->menubar->height()),
           QString::number(ui->menubar->height() * 2)));
  ui->menubar->setCornerWidget(toggleNav);
  toggleNav->setIcon(
    StyleManager::getInstance().awesome().icon(fa_solid, fa_compass));
  toggleNav->setIconSize(QSize(20, 20));

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

QModelIndex
MainWindow::syncSelectedSurah()
{
  QItemSelectionModel* select = ui->listViewSurahs->selectionModel();
  select->clearSelection();
  QModelIndex surah = m_surahListModel.index(m_currVerse.surah() - 1);
  select->select(surah,
                 QItemSelectionModel::Rows | QItemSelectionModel::Select);
  ui->listViewSurahs->scrollTo(surah, QAbstractItemView::PositionAtCenter);

  return surah;
}

void
MainWindow::activeVerseChanged()
{
  Verse curr = m_currVerse;
  if (curr.number() == 0)
    curr.setNumber(1);

  syncSelectedSurah();
  setCmbPageIdx(curr.page() - 1);
  setCmbJuzIdx(m_quranService->getVerseJuz(curr) - 1);

  if (curr.surahCount() != ui->cmbVerse->count())
    setVerseComboBoxRange();
  else
    setCmbVerseIdx(curr.number() - 1);
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
  m_verseValidator->setTop(m_currVerse.surahCount());

  // updates values in the combobox with the current surah verses
  m_internalVerseChange = true;
  ui->cmbVerse->clear();
  for (int i = 1; i <= m_currVerse.surahCount(); i++)
    ui->cmbVerse->addItem(QString::number(i), i);
  m_internalVerseChange = false;

  ui->cmbVerse->setValidator(m_verseValidator);
  setCmbVerseIdx(m_currVerse.number() - 1);
}

void
MainWindow::searchSurahTextChanged(const QString& arg1)
{
  if (arg1.isEmpty()) {
    m_surahListModel.setStringList(m_surahList);
    syncSelectedSurah();
  } else {
    QList<int> suggestions = m_quranService->searchSurahNames(arg1);
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
  m_navigator.navigateToSurah(s);
}

void
MainWindow::cmbPageChanged(int newIdx)
{
  if (m_internalPageChange) {
    return;
  }

  m_reader->gotoPage(newIdx + 1);
}

void
MainWindow::cmbVerseChanged(int newVerseIdx)
{
  if (newVerseIdx < 0 || m_internalVerseChange) {
    return;
  }

  int number = newVerseIdx + 1;
  int page = m_quranService->getVersePage(m_currVerse.surah(), number);
  m_navigator.navigateToVerse(Verse(page, m_currVerse.surah(), number));
}

void
MainWindow::cmbJuzChanged(int newJuzIdx)
{
  if (m_internalJuzChange) {
    return;
  }

  m_navigator.navigateToJuz(newJuzIdx + 1);
}

void
MainWindow::updateTrayTooltip(QMediaPlayer::PlaybackState state)
{
  if (state == QMediaPlayer::PlayingState) {
    m_systemTray->setTooltip(
      tr("Now playing: ") + m_playbackController->player()->reciterName() +
      " - " + tr("Surah ") +
      m_quranService->surahNames().at(m_currVerse.surah() - 1));
  } else
    m_systemTray->setTooltip(tr("Quran Companion"));
}

void
MainWindow::bookmarkCurrent()
{
  if (!m_bookmarkService->isBookmarked(m_currVerse))
    m_bookmarkService->addBookmark(m_currVerse, false);
}

void
MainWindow::actionUpdatesTriggered()
{
  m_versionChecker->checkUpdates();
}

void
MainWindow::missingQCF()
{
  QMessageBox::StandardButton btn = QMessageBox::question(
    this,
    tr("Files Missing"),
    tr("The selected font files are missing, would you like to download it?"));

  if (btn == QMessageBox::Yes) {
    actionDMTriggered();
    m_downloaderDlg->selectDownload(DownloadJob::Qcf);
  }
}

void
MainWindow::missingTafsir(int idx)
{
  QMessageBox::StandardButton btn = QMessageBox::question(
    this,
    tr("Files Missing"),
    tr("The selected tafsir is missing, would you like to download it?"));

  if (btn == QMessageBox::Yes) {
    actionDMTriggered();
    m_downloaderDlg->selectDownload(DownloadJob::TafsirFile, { 0, idx });
  }
}

void
MainWindow::missingTranslation(int idx)
{
  QMessageBox::StandardButton btn = QMessageBox::question(
    this,
    tr("Files Missing"),
    tr("The selected translation is missing, would you like to download it?"));

  if (btn == QMessageBox::Yes) {
    actionDMTriggered();
    m_downloaderDlg->selectDownload(DownloadJob::TranslationFile, { 1, idx });
  }
}

void
MainWindow::missingRecitationFileWarn(int reciterIdx, int surah)
{
  if (!m_config.settings().value("MissingFileWarning").toBool())
    return;

  QMessageBox::StandardButton btn =
    QMessageBox::question(this,
                          tr("Recitation not found"),
                          tr("The recitation files for the current surah is "
                             "missing, would you like to download it?"));

  if (btn == QMessageBox::Yes) {
    actionDMTriggered();
    m_downloaderDlg->selectDownload(DownloadJob::Recitation,
                                    { reciterIdx, surah });
  }
}

void
MainWindow::actionPrefTriggered()
{
  m_settingsDlg->showWindow();
}

void
MainWindow::actionDMTriggered()
{
  if (m_downloaderDlg == nullptr)
    m_downloaderDlg = new DownloaderDialog(this, m_jobMgr);

  m_downloaderDlg->show();
}

void
MainWindow::actionBookmarksTriggered()
{
  if (m_bookmarksDlg == nullptr)
    m_bookmarksDlg = new BookmarksDialog(this);

  m_bookmarksDlg->showWindow();
}

void
MainWindow::actionKhatmahTriggered()
{
  if (m_khatmahDlg == nullptr)
    m_khatmahDlg = new KhatmahDialog(this);

  m_khatmahService->saveActiveKhatmah(m_currVerse);
  m_khatmahDlg->show();
}

void
MainWindow::actionAdvancedCopyTriggered()
{

  m_cpyDlg->show();
}

void
MainWindow::actionTafsirTriggered()
{
  m_contentDlg->showVerseTafsir(m_currVerse);
}

void
MainWindow::actionVotdTriggered()
{
  m_verseDlg->showVOTD(false);
}

void
MainWindow::actionAboutTriggered()
{
  static AboutDialog* about = new AboutDialog(this);
  about->show();
}

void
MainWindow::actionAboutQttriggered()
{
  QMessageBox::aboutQt(this, tr("About Qt"));
}

void
MainWindow::actionSearchTriggered()
{
  if (m_searchDlg == nullptr)
    m_searchDlg = new SearchDialog(this);

  m_searchDlg->show();
}

void
MainWindow::actionPlayerControlsToggled(bool checked)
{
  m_playerControls->setVisible(checked);
  if (m_config.settings().value("Reader/AdaptiveFont").toBool()) {
    m_reader->redrawQuranPage();
    m_reader->highlightCurrentVerse();
  }
}

void
MainWindow::navigateToSurah(QModelIndex& index)
{
  int s = index.row() + 1;
  m_navigator.navigateToSurah(s);
}

void
MainWindow::nextVerse()
{
  m_navigator.navigateToNextVerse();
}

void
MainWindow::prevVerse()
{
  m_navigator.navigateToPreviousVerse();
}

void
MainWindow::nextJuz()
{
  m_navigator.navigateToNextJuz();
}

void
MainWindow::prevJuz()
{
  m_navigator.navigateToPreviousJuz();
}

void
MainWindow::nextSurah()
{
  m_navigator.navigateToNextSurah();
}

void
MainWindow::prevSurah()
{
  m_navigator.navigateToPreviousSurah();
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
MainWindow::importUserData()
{
  QString path = m_selectorDlg->selectJson(FileSelector::Read);
  if (!path.isEmpty())
    m_importExportDlg->selectImports(path);
}

void
MainWindow::exportUserData()
{
  QString path = m_selectorDlg->selectJson(FileSelector::Write);
  if (!path.isEmpty())
    m_importExportDlg->selectExports(path);
}

void
MainWindow::resizeEvent(QResizeEvent* event)
{
  QMainWindow::resizeEvent(event);
  m_popup->adjustLocation();
  m_popup->move(m_popup->notificationPos());
  m_betaqaViewer->center();
}

void
MainWindow::saveReaderState()
{
  m_config.settings().setValue("WindowState", saveState());
  m_config.settings().setValue("Reciter", m_playerControls->currentReciter());
  m_config.settings().sync();

  m_khatmahService->saveActiveKhatmah(m_currVerse);
}

void
MainWindow::restartApp()
{
  saveReaderState();
  QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
  emit QApplication::exit();
}

MainWindow::~MainWindow()
{
  saveReaderState();
  delete ui;
}
