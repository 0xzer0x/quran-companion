/**
 * @file settingsdialog.cpp
 * @brief Implementation file for SettingsDialog
 */

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QFileDialog>
#include <utils/fontmanager.h>
#include <utils/stylemanager.h>
#include <widgets/shortcutdelegate.h>

SettingsDialog::SettingsDialog(QWidget* parent, VersePlayer* vPlayerPtr)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , m_audioOutIdx(0)
  , m_vPlayerPtr(vPlayerPtr)
  , m_config(Configuration::getInstance())
  , m_downloadsDir(DirManager::getInstance().downloadsDir())
  , m_shortcutDescription(ShortcutHandler::getInstance().shortcutsDescription())
  , m_tafasir(Tafsir::tafasir)
  , m_translations(Translation::translations)
{
  ui->setupUi(this);
  ui->cmbQuranFontSz->setValidator(new QIntValidator(10, 72));
  ui->cmbSideFontSz->setValidator(new QIntValidator(10, 72));
  setWindowIcon(
    StyleManager::getInstance().awesome().icon(fa::fa_solid, fa::fa_gear));
  ui->tableViewShortcuts->setModel(&m_shortcutsModel);
  ui->tableViewShortcuts->horizontalHeader()->setStretchLastSection(true);
  ui->tableViewShortcuts->setItemDelegate(new ShortcutDelegate);
  ui->lbDownloadsPath->setText(m_downloadsDir.absolutePath());

  fillLanguageCombobox();
  setCurrentSettingsAsRef();
  setupConnections();
}

void
SettingsDialog::setupConnections()
{
  connect(ui->btnChangeDownloadsPath,
          &QPushButton::clicked,
          this,
          &SettingsDialog::selectDownloadsDir);
  connect(ui->buttonBox,
          &QDialogButtonBox::clicked,
          this,
          &SettingsDialog::btnBoxAction);
}

void
SettingsDialog::populateShortcutsModel()
{
  QStringList headers;
  headers << tr("Description") << tr("Key");
  m_shortcutsModel.clear();
  m_shortcutsModel.setHorizontalHeaderLabels(headers);

  foreach (const QString& key, m_shortcutDescription.keys()) {
    QStandardItem *desc = new QStandardItem(), *keySeq = new QStandardItem();
    desc->setData(key, Qt::UserRole);
    desc->setText(tr(m_shortcutDescription.value(key).toStdString().c_str()));
    keySeq->setText(m_config.settings().value("Shortcuts/" + key).toString());

    QList<QStandardItem*> row;
    row.append(desc);
    row.append(keySeq);
    m_shortcutsModel.appendRow(row);
  }
}

void
SettingsDialog::fillLanguageCombobox()
{
  ui->cmbLang->addItem("English", QLocale::English);
  ui->cmbLang->addItem("العربية", QLocale::Arabic);
  ui->cmbLang->addItem("Türkçe", QLocale::Turkish);
  ui->cmbLang->addItem("русскийe", QLocale::Russian);
  ui->cmbLang->addItem("Bahasa Indonesia", QLocale::Indonesian);
}

void
SettingsDialog::updateContentCombobox()
{
  ui->cmbTranslation->clear();
  foreach (const Translation& tr, m_translations) {
    if (tr.isAvailable())
      ui->cmbTranslation->addItem(tr.displayName(), tr.id());
  }

  m_translation = ui->cmbTranslation->findData(
    m_config.settings().value("Reader/Translation"));
  ui->cmbTranslation->setCurrentIndex(m_translation);
}

void
SettingsDialog::setCurrentSettingsAsRef()
{
  m_votd = m_config.settings().value("VOTD").toBool();
  m_fgHighlight = m_config.settings().value("Reader/FGHighlight").toBool();
  m_missingFileWarning =
    m_config.settings().value("MissingFileWarning").toBool();

  m_adaptive = m_config.settings().value("Reader/AdaptiveFont").toBool();
  m_quranFontSize =
    m_config.settings()
      .value("Reader/QCF" + QString::number(m_config.qcfVersion()) + "Size")
      .toInt();
  m_sideFont =
    qvariant_cast<QFont>(m_config.settings().value("Reader/SideContentFont"));

  m_verseType = m_config.settings().value("Reader/VerseType").toInt();
  m_verseFontSize = m_config.settings().value("Reader/VerseFontSize").toInt();

  m_audioDevices = QMediaDevices::audioOutputs();
  if (!m_audioDevices.isEmpty()) {
    ui->cmbAudioDevices->clear();
    for (int i = 0; i < m_audioDevices.length(); i++) {
      ui->cmbAudioDevices->addItem(m_audioDevices.at(i).description());
      if (m_audioDevices.at(i) == m_vPlayerPtr->getOutput()->device())
        m_audioOutIdx = i;
    }
    ui->cmbAudioDevices->setCurrentIndex(m_audioOutIdx);
  }

  // set ui elements to current settings
  ui->cmbLang->setCurrentIndex(ui->cmbLang->findData(m_config.language()));
  ui->cmbTheme->setCurrentIndex(m_config.themeId());
  ui->cmbReaderMode->setCurrentIndex(m_config.readerMode());
  ui->cmbQCF->setCurrentIndex(m_config.qcfVersion() - 1);
  ui->cmbQuranFontSz->setCurrentText(QString::number(m_quranFontSize));
  ui->fntCmbSide->setCurrentFont(m_sideFont);
  ui->cmbSideFontSz->setCurrentText(QString::number(m_sideFont.pointSize()));
  ui->chkDailyVerse->setChecked(m_votd);
  ui->chkAdaptive->setChecked(m_adaptive);
  ui->chkMissingWarning->setChecked(m_missingFileWarning);
  ui->chkFgHighlight->setChecked(m_fgHighlight);
  ui->cmbVerseText->setCurrentIndex(m_verseType);
  ui->cmbVersesFontSz->setCurrentText(QString::number(m_verseFontSize));

  // shortcuts tab
  populateShortcutsModel();
  updateContentCombobox();
  ui->tableViewShortcuts->resizeColumnsToContents();
}

void
SettingsDialog::checkShortcuts()
{
  for (int row = 0; row < m_shortcutsModel.rowCount(); row++) {
    const QString& key =
      m_shortcutsModel.item(row, 0)->data(Qt::UserRole).toString();
    const QString& keySequence = m_shortcutsModel.item(row, 1)->text();
    if (m_config.settings().value("Shortcuts/" + key).toString() != keySequence)
      updateShortcut(key, keySequence);
  }
}

void
SettingsDialog::selectDownloadsDir()
{
  QString path = QFileDialog::getExistingDirectory(
    this, tr("Select directory"), m_downloadsDir.absolutePath());
  if (path.isEmpty())
    return;

  QFileInfo pathInfo(path);
  if (!pathInfo.isWritable()) {
    QMessageBox::warning(
      this,
      tr("Invalid path"),
      tr("The chosen path is not valid. Please select a writable path."));
    return;
  }

  ui->lbDownloadsPath->setText(path);
}

void
SettingsDialog::updateTheme(int themeIdx)
{
  m_config.settings().setValue("Theme", themeIdx);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn =
    QMessageBox::question(this,
                          tr("Restart required"),
                          tr("Application theme was changed, restart now?"));

  m_restartReq = btn == QMessageBox::Yes;
}

void
SettingsDialog::updateLang(QLocale::Language lang)
{
  m_config.settings().setValue("Language", lang);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn =
    QMessageBox::question(this,
                          tr("Restart required"),
                          tr("Application language was changed, restart now?"));

  m_restartReq = btn == QMessageBox::Yes;
}

void
SettingsDialog::updateDownloadsPath(QString path)
{
  m_config.settings().setValue("DownloadsDir", path);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn = QMessageBox::question(
    this,
    tr("Restart required"),
    tr("Application downloads path was changed, restart now?"));

  m_restartReq = btn == QMessageBox::Yes;
}

void
SettingsDialog::updateDailyVerse(bool on)
{
  m_config.settings().setValue("VOTD", on);
}

void
SettingsDialog::updateFileWarning(bool on)
{
  m_config.settings().setValue("MissingFileWarning", on);
}

void
SettingsDialog::updateTranslation(QString id)
{
  m_config.settings().setValue("Reader/Translation", id);
  emit translationChanged();

  m_renderSideContent = true;
}

void
SettingsDialog::updateReaderMode(int idx)
{
  m_config.settings().setValue("Reader/Mode", idx);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn = QMessageBox::question(
    this, tr("Restart required"), tr("Reading mode was changed, restart now?"));

  m_restartReq = btn == QMessageBox::Yes;
}

void
SettingsDialog::updateQuranFont(int qcfV)
{
  if (qcfV == 2 && !FontManager::getInstance().getInstance().qcfExists()) {
    emit qcf2Missing();
    ui->cmbQCF->setCurrentIndex(0);
    return;
  }

  m_config.settings().setValue("Reader/QCF", qcfV);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn = QMessageBox::question(
    this,
    tr("Restart required"),
    tr("Restart is required to load new quran font, restart now?"));

  m_restartReq = btn == QMessageBox::Yes;
}

void
SettingsDialog::updateAdaptiveFont(bool on)
{
  m_config.settings().setValue("Reader/AdaptiveFont", on);
}

void
SettingsDialog::updateQuranFontSize(QString size)
{
  m_config.settings().setValue(
    "Reader/QCF" + QString::number(m_config.qcfVersion()) + "Size", size);
  emit quranFontChanged();
  m_renderQuranPage = true;
}

void
SettingsDialog::updateFgHighlight(bool on)
{
  m_config.settings().setValue("Reader/FGHighlight", on);
  emit highlightLayerChanged();
}

void
SettingsDialog::updateSideFont(QFont fnt)
{
  fnt.setPointSize(m_sideFont.pointSize());
  m_sideFont = fnt;

  m_config.settings().setValue("Reader/SideContentFont", m_sideFont);
  emit sideFontChanged();
  m_renderSideContent = true;
}

void
SettingsDialog::updateSideFontSize(QString size)
{
  m_sideFont.setPointSize(size.toInt());
  m_config.settings().setValue("Reader/SideContentFont", m_sideFont);
  emit sideFontChanged();
  m_renderSideContent = true;
}

void
SettingsDialog::updateVerseType(int vt)
{
  m_config.settings().setValue("Reader/VerseType", vt);
  emit verseTypeChanged();
  m_renderSideContent = true;
}

void
SettingsDialog::updateVerseFontsize(QString size)
{
  m_config.settings().setValue("Reader/VerseFontSize", size);
  emit verseTypeChanged();
  m_renderSideContent = true;
}

void
SettingsDialog::updateShortcut(QString key, QString keySequence)
{
  m_config.settings().setValue("Shortcuts/" + key, keySequence);
  emit shortcutChanged(key);
}

void
SettingsDialog::applyAllChanges()
{
  QLocale::Language chosenLang =
    qvariant_cast<QLocale::Language>(ui->cmbLang->currentData());
  if (chosenLang != m_config.language()) {
    updateLang(chosenLang);
  }

  if (ui->cmbTheme->currentIndex() != m_config.themeId())
    updateTheme(ui->cmbTheme->currentIndex());

  if (ui->lbDownloadsPath->text() != m_downloadsDir.absolutePath())
    updateDownloadsPath(ui->lbDownloadsPath->text());

  if (ui->chkDailyVerse->isChecked() != m_votd)
    updateDailyVerse(ui->chkDailyVerse->isChecked());

  if (ui->chkMissingWarning->isChecked() != m_missingFileWarning)
    updateFileWarning(ui->chkMissingWarning->isChecked());

  if (ui->chkFgHighlight->isChecked() != m_fgHighlight)
    updateFgHighlight(ui->chkFgHighlight->isChecked());

  if (ui->cmbTranslation->currentIndex() != m_translation)
    updateTranslation(ui->cmbTranslation->currentData().toString());

  if (ui->cmbQCF->currentIndex() + 1 != m_config.qcfVersion())
    updateQuranFont(ui->cmbQCF->currentIndex() + 1);

  if (ui->cmbVerseText->currentIndex() != m_verseType)
    updateVerseType(ui->cmbVerseText->currentIndex());

  if (ui->cmbVersesFontSz->currentText() != QString::number(m_verseFontSize))
    updateVerseFontsize(ui->cmbVersesFontSz->currentText());

  if (ui->chkAdaptive->isChecked() != m_adaptive)
    updateAdaptiveFont(ui->chkAdaptive->isChecked());

  if (ui->cmbReaderMode->currentIndex() != m_config.readerMode())
    updateReaderMode(ui->cmbReaderMode->currentIndex());

  bool forceManualFont = false;
  if (ui->cmbQuranFontSz->currentText() != QString::number(m_quranFontSize))
    updateQuranFontSize(ui->cmbQuranFontSz->currentText()),
      forceManualFont = true;

  if (ui->fntCmbSide->currentFont() != m_sideFont)
    updateSideFont(ui->fntCmbSide->currentFont());

  if (ui->cmbSideFontSz->currentText() !=
      QString::number(m_sideFont.pointSize()))
    updateSideFontSize(ui->cmbSideFontSz->currentText());

  if (!m_audioDevices.isEmpty() &&
      ui->cmbAudioDevices->currentIndex() != m_audioOutIdx) {
    m_audioOutIdx = ui->cmbAudioDevices->currentIndex();
    ui->cmbAudioDevices->setCurrentText(
      m_audioDevices.at(m_audioOutIdx).description());
    emit usedAudioDeviceChanged(m_audioDevices.at(m_audioOutIdx));
  }

  checkShortcuts();

  // restart after applying all changes if required
  if (m_restartReq) {
    emit restartApp();
    return;
  }

  // redraw once if flag is set
  if (m_renderQuranPage)
    emit redrawQuranPage(forceManualFont);

  if (m_renderSideContent)
    emit redrawSideContent();

  m_renderQuranPage = m_renderSideContent = false;
  setCurrentSettingsAsRef();
}

void
SettingsDialog::btnBoxAction(QAbstractButton* btn)
{
  if (btn == ui->buttonBox->button(QDialogButtonBox::StandardButton::Apply)) {
    applyAllChanges();
  } else if (btn ==
             ui->buttonBox->button(QDialogButtonBox::StandardButton::Cancel)) {
    this->reject();
  } else {
    applyAllChanges();
    this->accept();
  }
}

void
SettingsDialog::showWindow()
{
  setCurrentSettingsAsRef();
  this->show();
}

void
SettingsDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}
