/**
 * @file mainwindow.cpp
 * @brief Implementation file for MainWindow
 */

#include "mainwindow.h"
#include "../utils/stylemanager.h"
#include "../widgets/aboutdialog.h"
#include "khatmahdialog.h"
#include "ui_mainwindow.h"
#include <QtAwesome.h>
using namespace fa;

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_process(new QProcess(this))
  , m_verseValidator(new QIntValidator(this))
{
  ui->setupUi(this);
  loadIcons();
  loadVerse();
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
  m_dbMgr->setActiveKhatmah(id);
  if (!m_dbMgr->getKhatmahPos(id, vInfo)) {
    QString name = id ? tr("Khatmah ") + QString::number(id) : tr("Default");
    m_dbMgr->addKhatmah(vInfo, name, id);
  }

  m_currVerse->update(vInfo);
}

void
MainWindow::checkForUpdates()
{
#if defined Q_OS_WIN
  static QString updateTool = QApplication::applicationDirPath() +
                              QDir::separator() + "QCMaintenanceTool.exe";
  QFileInfo tool(updateTool);
  if (tool.exists()) {
    m_process->setWorkingDirectory(QApplication::applicationDirPath());
    m_process->start(updateTool, QStringList("ch"));
    return;
  }
#endif

  m_downManPtr->getLatestVersion();
}

void
MainWindow::updateProcessCallback()
{
  static QString updateTool = QApplication::applicationDirPath() +
                              QDir::separator() + "QCMaintenanceTool.exe";

  QString output = m_process->readAll();
  QString displayText;

  if (output.contains("There are currently no updates available.")) {
    displayText = tr("There are currently no updates available.");

    if (this->isVisible())
      QMessageBox::information(this, tr("Update info"), displayText);
    else
      m_systemTray->notify(tr("Update info"), displayText);
  }

  else {
    displayText = tr("Updates available, do you want to open the update tool?");
    if (this->isVisible()) {
      QMessageBox::StandardButton btn =
        QMessageBox::question(this, tr("Updates info"), displayText);
      if (btn == QMessageBox::Yes)
        m_process->startDetached(updateTool);
    }

    else {
      m_systemTray->notify(
        tr("Update info"),
        tr("Updates are available, use the maintainance tool to install "
           "the latest updates."));
    }
  }
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
  connect(
    ui->actionUpdates, &QAction::triggered, this, &MainWindow::checkForUpdates);
  connect(
    m_process, &QProcess::finished, this, &MainWindow::updateProcessCallback);
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
          this,
          &MainWindow::checkForUpdates);
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
  connect(m_downManPtr,
          &DownloadManager::downloadCompleted,
          m_popup,
          &NotificationPopup::completedDownload);
  connect(m_downManPtr,
          &DownloadManager::downloadErrored,
          m_popup,
          &NotificationPopup::downloadError);
  connect(m_downManPtr,
          &DownloadManager::latestVersionFound,
          m_popup,
          &NotificationPopup::checkUpdate);
  connect(m_cpyDlg,
          &CopyDialog::verseCopied,
          m_popup,
          &NotificationPopup::copiedToClipboard);
  connect(m_dbMgr.data(),
          &DBManager::bookmarkAdded,
          m_popup,
          &NotificationPopup::bookmarkAdded);
  connect(m_dbMgr.data(),
          &DBManager::bookmarkRemoved,
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
          &SettingsDialog::tafsirChanged,
          m_dbMgr.data(),
          &DBManager::updateLoadedTafsir);
  connect(m_settingsDlg,
          &SettingsDialog::translationChanged,
          m_dbMgr.data(),
          &DBManager::updateLoadedTranslation);
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
          this,
          &MainWindow::showVerseTafsir);
  connect(m_reader,
          &QuranReader::showBetaqa,
          m_betaqaViewer,
          &BetaqaViewer::showSurah);
}

void
MainWindow::init()
{
  m_player = new VersePlayer(this, m_settings->value("Reciter", 0).toInt());
  m_reader = new QuranReader(this, m_player);
  m_playerControls = new PlayerControls(this, m_player, m_reader);
  m_settingsDlg = new SettingsDialog(this, m_player);
  m_popup = new NotificationPopup(this);
  m_betaqaViewer = new BetaqaViewer(this);
  m_verseDlg = new VerseDialog(this);
  m_downManPtr = new DownloadManager(this);
  m_cpyDlg = new CopyDialog(this);

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
  m_systemTray = new SystemTray(this);

  setVerseComboBoxRange(true);

  for (int i = 1; i < 605; i++) {
    ui->cmbPage->addItem(QString::number(i));
  }

  // sets without emitting signal
  setCmbVerseIdx(m_currVerse->number() - 1);
  setCmbJuzIdx(m_dbMgr->getJuzOfPage(m_currVerse->page()) - 1);

  ui->cmbPage->setCurrentIndex(m_currVerse->page() - 1);
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
  selector->select(m_surahListModel.index(m_currVerse->surah() - 1),
                   QItemSelectionModel::Rows | QItemSelectionModel::Select);

  ui->listViewSurahs->scrollTo(m_surahListModel.index(m_currVerse->surah() - 1),
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
  setCmbJuzIdx(m_dbMgr->getJuzOfPage(m_currVerse->page()) - 1);
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
  ui->cmbVerse->clear();
  m_internalVerseChange = true;
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
  int page = m_dbMgr->getVersePage(m_currVerse->surah(), verse);

  if (page != m_currVerse->page())
    m_reader->gotoPage(page, false);

  m_currVerse->setPage(page);
  m_currVerse->setNumber(verse);
  m_reader->highlightCurrentVerse();

  setCmbPageIdx(page - 1);
  setCmbJuzIdx(m_dbMgr->getJuzOfPage(page) - 1);

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
  int page = m_dbMgr->getJuzStartPage(newJuzIdx + 1);
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
    m_systemTray->setTooltip(tr("Now playing: ") + m_player->reciterName() +
                             " - " + tr("Surah ") +
                             m_dbMgr->getSurahName(m_currVerse->surah()));
  } else
    m_systemTray->setTooltip(tr("Quran Companion"));
}

void
MainWindow::addCurrentToBookmarks()
{
  QList<int> vInfo = m_currVerse->toList();
  if (!m_dbMgr->isBookmarked(vInfo))
    m_dbMgr->addBookmark(vInfo);
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
    m_downloaderDlg->selectDownload(DownloadManager::QCF);
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
    m_downloaderDlg->selectDownload(DownloadManager::File, { 0, idx });
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
    m_downloaderDlg->selectDownload(DownloadManager::Recitation,
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

  m_dbMgr->saveActiveKhatmah(m_currVerse->toList());
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
  showVerseTafsir(*m_currVerse);
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
MainWindow::showVerseTafsir(Verse v)
{
  static bool reload = false;
  if (reload) {
    m_dbMgr->updateLoadedTafsir();
    reload = false;
  }

  if (!Tafsir::tafsirExists(m_dbMgr->currTafsir())) {
    int i;
    for (i = 0; i < m_tafasir.size(); i++)
      if (m_dbMgr->currTafsir() == m_tafasir[i])
        break;
    reload = true;
    return missingTafsir(i);
  }

  if (m_tafsirDlg == nullptr) {
    m_tafsirDlg = new TafsirDialog(this);
  }

  m_tafsirDlg->setShownVerse(v);
  m_tafsirDlg->loadVerseTafsir();
  m_tafsirDlg->show();
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

  m_dbMgr->saveActiveKhatmah(m_currVerse->toList());
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
