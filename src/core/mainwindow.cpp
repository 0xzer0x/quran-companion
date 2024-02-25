/**
 * @file mainwindow.cpp
 * @brief Implementation file for MainWindow
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtAwesome.h>
#include <dialogs/aboutdialog.h>
#include <dialogs/khatmahdialog.h>
#include <utils/stylemanager.h>
using namespace fa;

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_verseValidator(new QIntValidator(this))
{
  ui->setupUi(this);
  loadIcons();
  loadVerse();
  loadComponents();

  if (m_settings->value("WindowState").isNull())
    m_settings->setValue("WindowState", saveState());
  else
    restoreState(m_settings->value("WindowState").toByteArray());

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
  ui->actionKhatmah->setIcon(StyleManager::awesome->icon(fa_solid, fa_list));
  ui->actionDownloadManager->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_download));
  ui->actionExit->setIcon(StyleManager::awesome->icon(fa_solid, fa_xmark));
  ui->actionFind->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_magnifying_glass));
  ui->actionTafsir->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_book_open));
  ui->actionVOTD->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_calendar_day));
  ui->actionBookmarks->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_bookmark));
  ui->actionPereferences->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_gear));
  ui->actionAdvancedCopy->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_clipboard));
  ui->actionReaderViewToggle->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_columns));
  ui->actionUpdates->setIcon(
    StyleManager::awesome->icon(fa_solid, fa_arrow_rotate_right));
}

void
MainWindow::loadVerse()
{
  int id = m_settings->value("Reader/Khatmah").toInt();
  QList<int> vInfo = m_currVerse->toList();
  m_bookmarkDb->setActiveKhatmah(id);
  if (!m_bookmarkDb->loadVerse(id, vInfo)) {
    QString name = id ? tr("Khatmah ") + QString::number(id) : tr("Default");
    m_bookmarkDb->addKhatmah(vInfo, name, id);
  }

  m_currVerse->update(vInfo);
}

void
MainWindow::loadComponents()
{
  m_player = new VersePlayer(this, m_settings->value("Reciter", 0).toInt());
  m_reader = new QuranReader(this, m_player);
  m_playerControls = new PlayerControls(this, m_player, m_reader);
  m_settingsDlg = new SettingsDialog(this, m_player);
  m_popup = new NotificationPopup(this);
  m_betaqaViewer = new BetaqaViewer(this);
  m_verseDlg = new VerseDialog(this);
  m_cpyDlg = new CopyDialog(this);
  m_systemTray = new SystemTray(this);
  m_contentDlg = new ContentDialog(this);
  m_jobMgr = new JobManager(this);
  m_versionChecker = new VersionChecker(this);

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

  setVerseComboBoxRange(true);

  for (int i = 1; i < 605; i++) {
    ui->cmbPage->addItem(QString::number(i));
  }

  // sets without emitting signal
  setCmbVerseIdx(m_currVerse->number() - 1);
  setCmbJuzIdx(m_quranDb->getJuzOfPage(m_currVerse->page()) - 1);

  ui->cmbPage->setCurrentIndex(m_currVerse->page() - 1);
}

void
MainWindow::setupShortcuts()
{
  m_shortcutHandler->createShortcuts(this);

  connect(m_shortcutHandler.data(),
          &ShortcutHandler::toggleMenubar,
          this,
          &MainWindow::toggleMenubar);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::toggleNavDock,
          this,
          &MainWindow::toggleNavDock);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::bookmarkCurrent,
          this,
          &MainWindow::addCurrentToBookmarks);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::openDownloads,
          this,
          &MainWindow::actionDMTriggered);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::openBookmarks,
          this,
          &MainWindow::actionBookmarksTriggered);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::openKhatmah,
          this,
          &MainWindow::actionKhatmahTriggered);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::openSearch,
          this,
          &MainWindow::actionSearchTriggered);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::openSettings,
          this,
          &MainWindow::actionPrefTriggered);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::openTafsir,
          this,
          &MainWindow::actionTafsirTriggered);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::openAdvancedCopy,
          this,
          &MainWindow::actionAdvancedCopyTriggered);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::nextVerse,
          this,
          &MainWindow::nextVerse);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::prevVerse,
          this,
          &MainWindow::prevVerse);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::nextSurah,
          this,
          &MainWindow::nextSurah);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::prevSurah,
          this,
          &MainWindow::prevSurah);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::nextJuz,
          this,
          &MainWindow::nextJuz);
  connect(m_shortcutHandler.data(),
          &ShortcutHandler::prevJuz,
          this,
          &MainWindow::prevJuz);
}

void
MainWindow::setupConnections()
{
  // ########## Menubar ########## //
  connect(ui->actionExit, &QAction::triggered, this, &QApplication::exit);
  connect(ui->actionPereferences,
          &QAction::triggered,
          this,
          &MainWindow::actionPrefTriggered);
  connect(ui->actionDownloadManager,
          &QAction::triggered,
          this,
          &MainWindow::actionDMTriggered);
  connect(ui->actionFind,
          &QAction::triggered,
          this,
          &MainWindow::actionSearchTriggered);
  connect(ui->actionTafsir,
          &QAction::triggered,
          this,
          &MainWindow::actionTafsirTriggered);
  connect(ui->actionVOTD,
          &QAction::triggered,
          this,
          &MainWindow::actionVotdTriggered);
  connect(ui->actionAdvancedCopy,
          &QAction::triggered,
          this,
          &MainWindow::actionAdvancedCopyTriggered);
  connect(ui->actionBookmarks,
          &QAction::triggered,
          this,
          &MainWindow::actionBookmarksTriggered);
  connect(ui->actionKhatmah,
          &QAction::triggered,
          this,
          &MainWindow::actionKhatmahTriggered);
  connect(ui->actionAboutQC,
          &QAction::triggered,
          this,
          &MainWindow::actionAboutTriggered);
  connect(ui->actionUpdates,
          &QAction::triggered,
          this,
          &MainWindow::actionUpdatesTriggered);

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
  connect(m_reader,
          &QuranReader::currentVerseChanged,
          this,
          &MainWindow::currentVerseChanged);
  connect(m_reader,
          &QuranReader::currentSurahChanged,
          this,
          &MainWindow::currentSurahChanged);
  connect(m_playerControls,
          &PlayerControls::currentVerseChanged,
          this,
          &MainWindow::currentVerseChanged);
  connect(m_playerControls,
          &PlayerControls::currentSurahChanged,
          this,
          &MainWindow::currentSurahChanged);

  // ########## navigation dock ########## //
  connect(ui->lineEditSearchSurah,
          &QLineEdit::textChanged,
          this,
          &MainWindow::searchSurahTextChanged);
  connect(ui->listViewSurahs,
          &QListView::clicked,
          this,
          &MainWindow::listSurahNameClicked);

  // ########## system tray ########## //
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

  // ########## Notification Popup ########## //
  connect(ui->sideDock,
          &QDockWidget::dockLocationChanged,
          m_popup,
          &NotificationPopup::setDockArea);
  connect(m_jobMgr,
          &JobManager::jobCompleted,
          m_popup,
          &NotificationPopup::completedDownload);
  connect(m_jobMgr,
          &JobManager::jobFailed,
          m_popup,
          &NotificationPopup::downloadError);
  connect(m_versionChecker,
          &VersionChecker::versionFound,
          m_popup,
          &NotificationPopup::checkUpdate);
  connect(m_cpyDlg,
          &CopyDialog::verseCopied,
          m_popup,
          &NotificationPopup::copiedToClipboard);
  connect(m_bookmarkDb.data(),
          &BookmarksDb::bookmarkAdded,
          m_popup,
          &NotificationPopup::bookmarkAdded);
  connect(m_bookmarkDb.data(),
          &BookmarksDb::bookmarkRemoved,
          m_popup,
          &NotificationPopup::bookmarkRemoved);

  // ########## Settings Dialog ########## //
  // Restart signal
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
          m_translationDb.data(),
          &TranslationDb::updateLoadedTranslation);
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
          m_player,
          &VersePlayer::changeUsedAudioDevice);
  // shortcut change
  connect(m_settingsDlg,
          &SettingsDialog::shortcutChanged,
          m_shortcutHandler.data(),
          &ShortcutHandler::shortcutChanged);

  connect(m_player,
          &VersePlayer::missingVerseFile,
          this,
          &MainWindow::missingRecitationFileWarn);
  connect(m_player,
          &QMediaPlayer::playbackStateChanged,
          this,
          &MainWindow::updateTrayTooltip);
  connect(m_player,
          &QMediaPlayer::mediaStatusChanged,
          this,
          &MainWindow::mediaStatusChanged);

  connect(ui->actionReaderViewToggle,
          &QAction::triggered,
          m_reader,
          &QuranReader::toggleReaderView);
  connect(m_verseDlg,
          &VerseDialog::navigateToVerse,
          m_reader,
          &QuranReader::navigateToVerse);
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
MainWindow::setupSurahsDock()
{
  for (int i = 1; i < 115; i++) {
    QString item = QString::number(i).rightJustified(3, '0') + ' ' +
                   m_quranDb->surahNames().at(i - 1);
    m_surahList.append(item);
  }

  m_surahListModel.setStringList(m_surahList);
  ui->listViewSurahs->setModel(&m_surahListModel);

  QItemSelectionModel* selector = ui->listViewSurahs->selectionModel();
  selector->select(m_surahListModel.index(m_currVerse->surah() - 1),
                   QItemSelectionModel::Rows | QItemSelectionModel::Select);

  ui->listViewSurahs->scrollTo(m_surahListModel.index(m_currVerse->surah() - 1),
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
  toggleNav->setIcon(StyleManager::awesome->icon(fa_solid, fa_compass));
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
  QModelIndex surah = m_surahListModel.index(m_currVerse->surah() - 1);
  select->select(surah,
                 QItemSelectionModel::Rows | QItemSelectionModel::Select);
  ui->listViewSurahs->scrollTo(surah, QAbstractItemView::PositionAtCenter);

  return surah;
}

void
MainWindow::currentVerseChanged()
{
  setCmbVerseIdx(m_currVerse->number() - 1);
  setCmbPageIdx(m_currVerse->page() - 1);
  setCmbJuzIdx(m_quranDb->getJuzOfPage(m_currVerse->page()) - 1);
}

void
MainWindow::currentSurahChanged()
{
  setVerseComboBoxRange();
  syncSelectedSurah();
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
  m_verseValidator->setTop(m_currVerse->surahCount());

  // updates values in the combobox with the current surah verses
  m_internalVerseChange = true;
  ui->cmbVerse->clear();
  for (int i = 1; i <= m_currVerse->surahCount(); i++)
    ui->cmbVerse->addItem(QString::number(i), i);
  m_internalVerseChange = false;

  ui->cmbVerse->setValidator(m_verseValidator);
  setCmbVerseIdx(m_currVerse->number() - 1);
}

void
MainWindow::searchSurahTextChanged(const QString& arg1)
{
  if (arg1.isEmpty()) {
    m_surahListModel.setStringList(m_surahList);
    syncSelectedSurah();
  } else {
    QList<int> suggestions = m_quranDb->searchSurahNames(arg1);
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
  m_reader->gotoSurah(s);
}

void
MainWindow::cmbPageChanged(int newIdx)
{
  if (m_internalPageChange) {
    qDebug() << "Internal page change";
    return;
  }

  m_reader->gotoPage(newIdx + 1);
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
  int page = m_quranDb->getVersePage(m_currVerse->surah(), verse);

  if (page != m_currVerse->page())
    m_reader->gotoPage(page, false);

  m_currVerse->setPage(page);
  m_currVerse->setNumber(verse);
  m_reader->highlightCurrentVerse();

  setCmbPageIdx(page - 1);
  setCmbJuzIdx(m_quranDb->getJuzOfPage(page) - 1);

  // open newly set verse recitation file
  m_player->loadActiveVerse();
}

void
MainWindow::cmbJuzChanged(int newJuzIdx)
{
  if (m_internalJuzChange) {
    qDebug() << "Internal jozz change";
    return;
  }
  int page = m_quranDb->getJuzStartPage(newJuzIdx + 1);
  m_reader->gotoPage(page);
}

void
MainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
  if (status == QMediaPlayer::EndOfMedia)
    nextVerse();
}

void
MainWindow::updateTrayTooltip(QMediaPlayer::PlaybackState state)
{
  if (state == QMediaPlayer::PlayingState) {
    m_systemTray->setTooltip(
      tr("Now playing: ") + m_player->reciterName() + " - " + tr("Surah ") +
      m_quranDb->surahNames().at(m_currVerse->surah() - 1));
  } else
    m_systemTray->setTooltip(tr("Quran Companion"));
}

void
MainWindow::addCurrentToBookmarks()
{
  QList<int> vInfo = m_currVerse->toList();
  if (!m_bookmarkDb->isBookmarked(vInfo))
    m_bookmarkDb->addBookmark(vInfo);
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
  if (!m_settings->value("MissingFileWarning").toBool())
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
  if (m_bookmarksDlg == nullptr) {
    m_bookmarksDlg = new BookmarksDialog(this);
    connect(m_bookmarksDlg,
            &BookmarksDialog::navigateToVerse,
            m_reader,
            &QuranReader::navigateToVerse);
  }

  m_bookmarksDlg->showWindow();
}

void
MainWindow::actionKhatmahTriggered()
{
  if (m_khatmahDlg == nullptr) {
    m_khatmahDlg = new KhatmahDialog(this);
    connect(m_khatmahDlg,
            &KhatmahDialog::navigateToVerse,
            m_reader,
            &QuranReader::navigateToVerse);
  }

  m_bookmarkDb->saveActiveKhatmah(m_currVerse->toList());
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
  m_contentDlg->showVerseTafsir(*m_currVerse);
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
MainWindow::on_actionAboutQt_triggered()
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
            m_reader,
            &QuranReader::navigateToVerse);
  }

  m_searchDlg->show();
}

void
MainWindow::navigateToSurah(QModelIndex& index)
{
  int s = index.row() + 1;
  m_reader->gotoSurah(s);
}

void
MainWindow::nextVerse()
{
  if (m_currVerse->surah() == 114 && m_currVerse->number() == 6)
    return;

  bool keepPlaying = m_player->isOn();
  m_reader->navigateToVerse(m_currVerse->next());
  if (keepPlaying)
    m_player->play();
}

void
MainWindow::prevVerse()
{
  if (m_currVerse->surah() == 1 && m_currVerse->number() == 1)
    return;

  bool keepPlaying = m_player->isOn();
  m_reader->navigateToVerse(m_currVerse->prev());
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
  if (m_currVerse->surah() < 114)
    m_reader->gotoSurah(m_currVerse->surah() + 1);
}

void
MainWindow::prevSurah()
{
  if (m_currVerse->surah() > 1)
    m_reader->gotoSurah(m_currVerse->surah() - 1);
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
  m_settings->setValue("WindowState", saveState());
  m_settings->setValue("Reciter", m_playerControls->currentReciter());
  m_settings->sync();

  m_bookmarkDb->saveActiveKhatmah(m_currVerse->toList());
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
