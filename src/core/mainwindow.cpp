#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent, QSettings *settingsPtr)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_settingsPtr = settingsPtr;
    init();

    if (m_settingsPtr->value("WindowState").isNull())
        m_settingsPtr->setValue("WindowState", saveState());
    else
        restoreState(m_settingsPtr->value("WindowState").toByteArray());

    QShortcut *spaceKey = new QShortcut(Qt::Key_Space, this);

    /* ------------------ UI connectors ------------------ */

    // Menubar
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::exit);
    connect(ui->actionPereferences, &QAction::triggered, this, &MainWindow::actionPrefTriggered);
    connect(ui->actionDownload_manager, &QAction::triggered, this, &MainWindow::actionDMTriggered);
    connect(ui->actionFind, &QAction::triggered, this, &MainWindow::openSearchDialog);

    // page controls
    connect(ui->btnNext, &QPushButton::clicked, this, &MainWindow::nextPage);
    connect(ui->btnPrev, &QPushButton::clicked, this, &MainWindow::prevPage);
    connect(ui->cmbSurah, &QComboBox::currentIndexChanged, this, &MainWindow::cmbSurahChanged);
    connect(ui->cmbPage, &QComboBox::currentIndexChanged, this, &MainWindow::cmbPageChanged);
    connect(ui->cmbVerse, &QComboBox::currentIndexChanged, this, &MainWindow::cmbVerseChanged);
    connect(m_player, &VersePlayer::newSurah, this, &MainWindow::updateSurah);
    connect(m_player, &VersePlayer::newVerse, this, &MainWindow::activeVerseChanged);
    connect(m_player, &VersePlayer::missingVerseFile, this, &MainWindow::missingRecitationFileWarn);

    // audio slider
    connect(m_player, &QMediaPlayer::positionChanged, this, &MainWindow::mediaPosChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &MainWindow::mediaStateChanged);
    connect(ui->sldrAudioPlayer, &QSlider::valueChanged, m_player, &QMediaPlayer::setPosition);

    // player control
    connect(ui->btnPlay, &QPushButton::clicked, this, &MainWindow::btnPlayClicked);
    connect(ui->btnPause, &QPushButton::clicked, this, &MainWindow::btnPauseClicked);
    connect(ui->btnStop, &QPushButton::clicked, this, &MainWindow::btnStopClicked);
    connect(ui->cmbReciter, &QComboBox::currentIndexChanged, m_player, &VersePlayer::changeReciter);
    connect(spaceKey, &QShortcut::activated, this, &MainWindow::spaceKeyPressed);
    connect(ui->btnSearch, &QPushButton::clicked, this, &MainWindow::openSearchDialog);
    connect(ui->btnPreferences, &QPushButton::clicked, this, &MainWindow::actionPrefTriggered);
}

void MainWindow::init()
{
    m_darkMode = m_settingsPtr->value("Theme").toInt() == 1;

    if (m_settingsPtr->value("Language").toString() == "العربية") {
        ui->frmCenteralCont->setLayoutDirection(Qt::LeftToRight);
        ui->retranslateUi(this);
    }
    setWindowTitle(QApplication::applicationName());

    m_settingsPtr->beginGroup("Reader");

    m_currVerse.page = m_settingsPtr->value("Page").toInt();
    m_currVerse.surah = m_settingsPtr->value("Surah").toInt();
    m_currVerse.number = m_settingsPtr->value("Verse").toInt();
    if (m_settingsPtr->value("SideContent").isNull()) {
        m_settingsPtr->setValue("SideContent", (int) SideContent::translation);
        m_settingsPtr->setValue("Tafsir", (int) DBManager::Tafsir::muyassar);
        m_settingsPtr->setValue("Translation", (int) DBManager::Translation::en_sahih);
    }
    m_settingsPtr->endGroup();

    // initalization
    m_dbManPtr = new DBManager(this);
    m_player = new VersePlayer(this, m_dbManPtr, m_currVerse);
    m_pageCon = new PageConstructor(this, m_dbManPtr, m_settingsPtr);

    updateQuranFontSize();
    updateHighlightColor();
    updateSideContentType();
    updateLoadedTafsir();
    updateLoadedTranslation();
    updateSideFont();

    redrawQuranPage();
    updateVerseDropDown();

    QVBoxLayout *vbl = new QVBoxLayout();
    vbl->setDirection(QBoxLayout::BottomToTop);
    ui->scrlVerseCont->setLayout(vbl);
    addSideContent();

    m_highlighter = new QTextCursor(ui->tdQuranPage->document());

    ui->menuView->addAction(ui->dockControls->toggleViewAction());
    ui->btnPlay->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
    ui->btnPause->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaPause));
    ui->btnStop->setIcon(QApplication::style()->standardIcon(QStyle::SP_MediaStop));
    ui->actionExit->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarCloseButton));

    QString st
        = "%1 { "
          "background-color:transparent;border-radius:6px;border-width:2px;border-style:solid; }";
    ui->tdQuranPage->setStyleSheet(st.arg("QFrame#tdQuranPage"));
    ui->scrlVerseByVerse->setStyleSheet(st.arg("QScrollArea#scrlVerseByVerse"));

    for (int i = 1; i < 605; i++) {
        ui->cmbPage->addItem(QString::number(i));
    }

    m_internalSurahChange = true;
    m_internalVerseChange = true;
    ui->cmbSurah->setCurrentIndex(m_currVerse.surah - 1);
    ui->cmbVerse->setCurrentIndex(m_currVerse.number - 1);
    m_internalSurahChange = false;
    m_internalVerseChange = false;

    ui->cmbPage->setCurrentIndex(m_currVerse.page - 1);
}

/* ------------------------ UI updating ------------------------ */

void MainWindow::updateSurah()
{
    ui->cmbSurah->setCurrentIndex(m_player->surahIdx() - 1);
}

void MainWindow::updatePageVerseInfoList()
{
    m_vInfoList = m_dbManPtr->getVerseInfoList(m_currVerse.page);
}

void MainWindow::updateVerseDropDown()
{
    m_internalVerseChange = true;

    m_player->updateSurahVerseCount();
    // updates values in the combobox with the current surah verses
    ui->cmbVerse->clear();
    for (int i = 1; i <= m_player->surahCount(); i++)
        ui->cmbVerse->addItem(QString::number(i), i);

    ui->cmbVerse->setCurrentIndex(m_currVerse.number - 1);

    m_internalVerseChange = false;
}

/* ------------------------ Page navigation ------------------------ */

void MainWindow::gotoPage(int page)
{
    m_currVerse.page = page;
    redrawQuranPage();

    btnStopClicked(); // stop playback, set verse & surah in player to the page selected
    addSideContent();
    counter = 0;
}

void MainWindow::nextPage()
{
    bool keepPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
    if (m_currVerse.page < 604) {
        ui->cmbPage->setCurrentIndex(m_currVerse.page);

        // if the page is flipped automatically, keep
        if (keepPlaying)
            btnPlayClicked();
    }
}

void MainWindow::prevPage()
{
    bool keepPlaying = m_player->playbackState() == QMediaPlayer::PlayingState;
    if (m_currVerse.page > 1) {
        ui->cmbPage->setCurrentIndex(m_currVerse.page - 2);

        if (keepPlaying)
            btnPlayClicked();
    }
}

void MainWindow::gotoSurah(int surahIdx)
{
    // getting surah index
    m_currVerse.page = m_dbManPtr->getSurahStartPage(surahIdx);
    m_currVerse.surah = ui->cmbSurah->currentIndex() + 1;
    m_currVerse.number = 1;

    // setting up the page of verse 1
    redrawQuranPage();
    addSideContent();

    // syncing the player & playing basmalah
    m_player->setVerse(m_currVerse);
    m_player->playBasmalah();

    m_internalPageChange = true;
    ui->cmbPage->setCurrentIndex(m_currVerse.page - 1);
    m_internalPageChange = false;

    m_player->updateSurahVerseCount();
    updateVerseDropDown();

    m_endOfPage = false;
}

void MainWindow::cmbPageChanged(int newIdx)
{
    if (m_internalPageChange) {
        qDebug() << "Internal page change";
        return;
    }

    gotoPage(newIdx + 1);

}

void MainWindow::cmbSurahChanged(int newSurahIdx)
{
    if (m_internalSurahChange) {
        qDebug() << "Internal surah change";
        return;
    }

    m_internalPageChange = true;

    gotoSurah(newSurahIdx + 1);

    m_internalPageChange = false;
}

void MainWindow::cmbVerseChanged(int newVerseIdx)
{
    if (newVerseIdx < 0)
        return;

    if (m_internalVerseChange) {
        qDebug() << "internal verse change";
        return;
    }

    int verse = newVerseIdx + 1;
    m_currVerse.page = m_dbManPtr->getVersePage(m_currVerse.surah, verse);
    m_currVerse.number = verse;

    redrawQuranPage();

    // update the player surah & verse
    m_player->setVerse(m_currVerse);
    // open newly set verse recitation file
    m_player->setVerseFile(m_player->constructVerseFilename());

    m_internalPageChange = true;
    ui->cmbPage->setCurrentIndex(m_currVerse.page - 1);
    m_internalPageChange = false;

    addSideContent();
    m_endOfPage = false;
}

/* ------------------------ Player controls / highlighting ------------------------ */

void MainWindow::spaceKeyPressed()
{
    if (m_player->playbackState() == QMediaPlayer::PlayingState) {
        btnPauseClicked();
    } else {
        btnPlayClicked();
    }
}

void MainWindow::btnPlayClicked()
{
    activeVerseChanged(); // to highlight the verse on pressing play
    m_player->play();
}

void MainWindow::btnPauseClicked()
{
  m_player->pause();
}

void MainWindow::btnStopClicked()
{
  m_player->stop();

  // set the current surah / verse to the surah / verse at the top of the page
  m_currVerse.surah = m_vInfoList.at(0).value("surah");
  m_currVerse.number = m_vInfoList.at(0).value("ayah");
  // update the player surah & verse
  m_player->setVerse(m_currVerse);
  // open newly set verse recitation file
  m_player->setVerseFile(m_player->constructVerseFilename());

  m_internalSurahChange = true;
  updateSurah();
  updateVerseDropDown();
  m_internalSurahChange = false;

  m_endOfPage = false;
}

void MainWindow::mediaStateChanged(QMediaPlayer::PlaybackState state)
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

void MainWindow::mediaPosChanged(qint64 position)
{
  if (ui->sldrAudioPlayer->maximum() != m_player->duration())
        ui->sldrAudioPlayer->setMaximum(m_player->duration());

  ui->sldrAudioPlayer->setValue(position);
}

void MainWindow::missingRecitationFileWarn()
{
  QMessageBox::StandardButton btn = QMessageBox::question(
      this,
      tr("Recitation not found"),
      tr("The recitation files for the current surah is missing, would you like to download it?"));

  if (btn == QMessageBox::Yes) {
        actionDMTriggered();
  }
}

void MainWindow::activeVerseChanged()
{
  m_internalVerseChange = true;

  m_currVerse.surah = m_player->surahIdx();
  m_currVerse.number = m_player->verseNum();

  if (m_currVerse.number == 0)
        m_currVerse.number = 1;

  ui->cmbVerse->setCurrentIndex(m_currVerse.number - 1);
  m_internalVerseChange = false;

  if (m_endOfPage) {
        m_endOfPage = false;
        nextPage();
  }

  // If now playing the last verse in the page, set the flag to flip the page
  if (m_currVerse.number == m_vInfoList.at(m_vInfoList.count() - 1).value("ayah")
      && m_currVerse.number != m_player->surahCount()) {
        m_endOfPage = true;
  }

  highlightCurrentVerse();
}

void MainWindow::verseClicked()
{
  // object = clickable label, parent = verse frame, verse frame name scheme = 'surah_verse'
  QStringList data = sender()->parent()->objectName().split('_');
  int sNum = data.at(0).toInt();
  int vNum = data.at(1).toInt();

  if (m_currVerse.surah != sNum) {
        m_currVerse.surah = sNum;
        m_player->updateSurahVerseCount();
        updateVerseDropDown();

        m_internalSurahChange = true;
        ui->cmbSurah->setCurrentIndex(sNum - 1);
        m_internalSurahChange = false;
  }

  m_internalVerseChange = true;
  cmbVerseChanged(vNum - 1);
  m_internalVerseChange = false;

  if (m_settingsPtr->value("Reader/CopyVerseOnClick").toBool()) {
        QClipboard *clip = QApplication::clipboard();
        clip->setText(m_dbManPtr->getVerseText(sNum, vNum));
  }

  m_endOfPage = false;
  m_currVerse.number = vNum;
  m_player->setVerse(m_currVerse);
  m_player->setVerseFile(m_player->constructVerseFilename());
  btnPlayClicked();
}

void MainWindow::highlightCurrentVerse()
{
  QTextCharFormat tcf;
  tcf.setForeground(m_pageCon->pFmt().foreground());

  if (m_highlighter->hasSelection())
        m_highlighter->mergeCharFormat(tcf); // de-highlight any previous highlights

  tcf.setForeground(m_highlightColor);

  QList<int> bounds = m_dbManPtr->getVerseBounds(m_currVerse.surah, m_currVerse.number);

  m_highlighter->setPosition(bounds.at(0));
  m_highlighter->setPosition(bounds.at(1), QTextCursor::KeepAnchor);
  m_highlighter->mergeCharFormat(tcf);

  qInfo() << "Selection start:" << m_highlighter->selectionStart()
          << " Selection end:" << m_highlighter->selectionEnd();

  if (m_highlightedFrm != nullptr)
        m_highlightedFrm->setStyleSheet("");

  VerseFrame *verseFrame = ui->scrlVerseCont->findChild<VerseFrame *>(
      QString("%0_%1").arg(m_currVerse.surah).arg(m_currVerse.number));

  verseFrame->highlightFrame();

  if (m_highlightedFrm != nullptr) {
        ui->scrlVerseByVerse->verticalScrollBar()->setValue(m_highlightedFrm->pos().y());
  }

  m_highlightedFrm = verseFrame;
}

/* ------------------------ Settings update methods ------------------------ */

void MainWindow::actionPrefTriggered()
{
  if (m_settingsDlg != nullptr)
        delete m_settingsDlg;

  m_settingsDlg = new SettingsDialog(this, m_settingsPtr, m_player);

  // Quran page signals
  connect(m_settingsDlg, &SettingsDialog::redrawQuranPage, this, &MainWindow::redrawQuranPage);
  connect(m_settingsDlg, &SettingsDialog::quranFontChanged, this, &MainWindow::updateQuranFontSize);

  // Side panel signals
  connect(m_settingsDlg, &SettingsDialog::redrawSideContent, this, &MainWindow::addSideContent);
  connect(m_settingsDlg,
          &SettingsDialog::sideContentTypeChanged,
          this,
          &MainWindow::updateSideContentType);
  connect(m_settingsDlg, &SettingsDialog::tafsirChanged, this, &MainWindow::updateLoadedTafsir);
  connect(m_settingsDlg,
          &SettingsDialog::translationChanged,
          this,
          &MainWindow::updateLoadedTranslation);
  connect(m_settingsDlg, &SettingsDialog::sideFontChanged, this, &MainWindow::updateSideFont);

  // audio device signals
  connect(m_settingsDlg,
          &SettingsDialog::usedAudioDeviceChanged,
          m_player,
          &VersePlayer::changeUsedAudioDevice);

  m_settingsDlg->show();
}

void MainWindow::actionDMTriggered()
{
  if (m_downloaderDlg == nullptr) {
        if (m_downManPtr == nullptr)
            m_downManPtr = new DownloadManager(this, m_dbManPtr, m_player->reciterDirNames());

        m_downloaderDlg = new DownloaderDialog(this, m_settingsPtr, m_downManPtr, m_dbManPtr);
  }

  m_downloaderDlg->show();
}

void MainWindow::redrawQuranPage()
{
  ui->tdQuranPage->clear();
  m_pageCon->constructDoc(m_currVerse.page, m_darkMode);
  ui->tdQuranPage->setDocument(m_pageCon->pageTextD());
  updatePageVerseInfoList();
}

void MainWindow::updateSideContentType()
{
  m_sideContent = static_cast<SideContent>(m_settingsPtr->value("Reader/SideContent").toInt());
}

void MainWindow::updateLoadedTafsir()
{
  DBManager::Tafsir currTafsir = static_cast<DBManager::Tafsir>(
      m_settingsPtr->value("Reader/Tafsir").toInt());

  m_dbManPtr->setCurrentTafsir(currTafsir);
}

void MainWindow::updateLoadedTranslation()
{
  DBManager::Translation currTrans = static_cast<DBManager::Translation>(
      m_settingsPtr->value("Reader/Translation").toInt());

  m_dbManPtr->setCurrentTranslation(currTrans);
}

void MainWindow::updateSideFont()
{
  m_sideFont = qvariant_cast<QFont>(m_settingsPtr->value("Reader/SideContentFont"));
}

void MainWindow::updateQuranFontSize()
{
  QString quranFontSize = m_settingsPtr->value("Reader/QuranFontSize").toString();

  m_pageCon->setFontSize(quranFontSize.toInt());
}

void MainWindow::updateHighlightColor()
{
  QColor clr(0, 161, 185);
  m_highlightColor = QBrush(clr);
}

/* ------------------------ Side content generation ------------------------ */

void MainWindow::addSideContent()
{
  if (!m_verseFrameList.isEmpty()) {
        qDeleteAll(m_verseFrameList);
        m_verseFrameList.clear();
        m_highlightedFrm = nullptr;
  }

  bool showTafsir = m_sideContent == SideContent::tafsir;
  bool showFullTafsir = m_settingsPtr->value("Reader/Tafsir").toInt()
                        == DBManager::Tafsir::muyassar;

  ClickableLabel *verselb;
  ClickableLabel *contentLb;
  VerseFrame *verseContFrame;
  QString prevLbContent, currLbContent;
  for (int i = m_vInfoList.size() - 1; i >= 0; i--) {
        QMap<QString, int> vInfo = m_vInfoList.at(i);

        verseContFrame = new VerseFrame(ui->scrlVerseCont);
        verselb = new ClickableLabel(verseContFrame);
        contentLb = new ClickableLabel(verseContFrame);

        verseContFrame->setObjectName(
            QString("%0_%1").arg(vInfo.value("surah")).arg(vInfo.value("ayah")));

        verselb->setFont(QFont(m_pageCon->pageFont(), m_pageCon->fontSize() - 2));
        verselb->setText(m_dbManPtr->getVerseGlyphs(vInfo.value("surah"), vInfo.value("ayah")));
        verselb->setAlignment(Qt::AlignCenter);
        verselb->setWordWrap(true);

        if (showTafsir) {
            QString txt = m_dbManPtr->getTafsir(vInfo.value("surah"), vInfo.value("ayah"));
            if (showFullTafsir)
                currLbContent = txt;
            else {
                currLbContent = tr("Expand...");
                connect(contentLb,
                        &ClickableLabel::clicked,
                        this,
                        &MainWindow::showExpandedVerseTafsir);
            }

        } else {
            currLbContent = m_dbManPtr->getTranslation(vInfo.value("surah"), vInfo.value("ayah"));
        }

        if (currLbContent == prevLbContent && (!showTafsir || showFullTafsir)) {
            currLbContent.clear();
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

void MainWindow::showExpandedVerseTafsir()
{
  ClickableLabel *showLb = qobject_cast<ClickableLabel *>(sender());

  if (showLb->text() == tr("Expand...")) {
        QStringList data = sender()->parent()->objectName().split('_');
        QString text = m_dbManPtr->getTafsir(data.at(0).toInt(), data.at(1).toInt());
        showLb->setText(text);
  } else
        showLb->setText(tr("Expand..."));
}

void MainWindow::openSearchDialog()
{
  if (m_srchDlg == nullptr) {
        m_srchDlg = new SearchDialog(this, m_dbManPtr);
        connect(m_srchDlg, &SearchDialog::navigateToVerse, this, &MainWindow::navigateToVerse);
  }

  m_srchDlg->show();
}

void MainWindow::navigateToVerse(Verse v)
{
  m_currVerse = v;

  redrawQuranPage();
  addSideContent();

  m_player->setVerse(m_currVerse);
  m_player->updateSurahVerseCount();
  m_player->setVerseFile(m_player->constructVerseFilename());
  updateVerseDropDown();

  m_internalPageChange = true;
  ui->cmbPage->setCurrentIndex(m_currVerse.page - 1);
  m_internalPageChange = false;

  m_internalSurahChange = true;
  ui->cmbSurah->setCurrentIndex(m_currVerse.surah - 1);
  m_internalSurahChange = false;

  m_internalVerseChange = true;
  ui->cmbVerse->setCurrentIndex(m_currVerse.number - 1);
  m_internalVerseChange = false;

  highlightCurrentVerse();
  m_endOfPage = false;
}

MainWindow::~MainWindow()
{
  m_settingsPtr->setValue("WindowState", saveState());
  m_settingsPtr->beginGroup("Reader");
  m_settingsPtr->setValue("Page", m_currVerse.page);
  m_settingsPtr->setValue("Surah", m_currVerse.surah);
  m_settingsPtr->setValue("Verse", m_currVerse.number);
  m_settingsPtr->endGroup();
  delete ui;
}
