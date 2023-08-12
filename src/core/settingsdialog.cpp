/**
 * @file settingsdialog.cpp
 * @brief Implementation file for SettingsDialog
 */

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent, VersePlayer* vPlayerPtr)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , m_vPlayerPtr{ vPlayerPtr }
{
  ui->setupUi(this);
  ui->cmbQuranFontSz->setValidator(new QIntValidator(10, 100));
  ui->cmbSideFontSz->setValidator(new QIntValidator(10, 100));
  setWindowIcon(QIcon(m_resources.filePath("icons/prefs.png")));
  fillLanguageCombobox();

  setCurrentSettingsAsRef();
  // connectors
  setupConnections();
}

void
SettingsDialog::setupConnections()
{
  connect(ui->buttonBox,
          &QDialogButtonBox::clicked,
          this,
          &SettingsDialog::btnBoxAction);
}

void
SettingsDialog::fillLanguageCombobox()
{
  ui->cmbLang->addItem("English", QLocale::English);
  ui->cmbLang->addItem("العربية", QLocale::Arabic);
}

void
SettingsDialog::setCurrentSettingsAsRef()
{
  m_votd = m_settings->value("VOTD").toBool();
  m_missingFileWarning = m_settings->value("MissingFileWarning").toBool();

  m_settings->beginGroup("Reader");
  // all keys have prefix "Reader"
  m_adaptive = m_settings->value("AdaptiveFont").toBool();
  m_quranFontSize =
    m_settings->value("QCF" + QString::number(m_qcfVer) + "Size").toInt();
  m_sideFont = qvariant_cast<QFont>(m_settings->value("SideContentFont"));
  m_tafsir = m_settings->value("Tafsir").toInt();
  m_trans = m_settings->value("Translation").toInt();
  m_settings->endGroup();

  m_audioDevices = QMediaDevices::audioOutputs();
  ui->cmbAudioDevices->clear();
  for (int i = 0; i < m_audioDevices.length(); i++) {
    ui->cmbAudioDevices->addItem(m_audioDevices.at(i).description());
    if (m_audioDevices.at(i) == m_vPlayerPtr->getOutput()->device())
      m_audioOutIdx = i;
  }

  // set ui elements to current settings
  ui->cmbAudioDevices->setCurrentIndex(m_audioOutIdx);
  ui->cmbLang->setCurrentIndex(ui->cmbLang->findData(m_languageCode));
  ui->cmbTheme->setCurrentIndex(m_themeIdx);
  ui->cmbQCF->setCurrentIndex(m_qcfVer - 1);
  ui->cmbQuranFontSz->setCurrentText(QString::number(m_quranFontSize));
  ui->fntCmbSide->setCurrentFont(m_sideFont);
  ui->cmbSideFontSz->setCurrentText(QString::number(m_sideFont.pointSize()));
  ui->cmbTafsir->setCurrentIndex(m_tafsir);
  ui->cmbTranslation->setCurrentIndex(m_trans);
  ui->cmbAudioDevices->setCurrentIndex(m_audioOutIdx);
  ui->chkDailyVerse->setChecked(m_votd);
  ui->chkAdaptive->setChecked(m_adaptive);
  ui->chkMissingWarning->setChecked(m_missingFileWarning);
}

void
SettingsDialog::updateTheme(int themeIdx)
{
  m_settings->setValue("Theme", themeIdx);
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
  m_settings->setValue("Language", lang);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn =
    QMessageBox::question(this,
                          tr("Restart required"),
                          tr("Application language was changed, restart now?"));

  m_restartReq = btn == QMessageBox::Yes;
}

void
SettingsDialog::updateDailyVerse(bool on)
{
  m_settings->setValue("VOTD", on);
}

void
SettingsDialog::updateFileWarning(bool on)
{
  m_settings->setValue("MissingFileWarning", on);
}

void
SettingsDialog::updateTafsir(int idx)
{
  m_settings->setValue("Reader/Tafsir", idx);
  emit tafsirChanged();
}

void
SettingsDialog::updateTranslation(int idx)
{
  m_settings->setValue("Reader/Translation", idx);
  emit translationChanged();

  m_renderSideContent = true;
}

void
SettingsDialog::updateQuranFont(int qcfV)
{
  m_settings->setValue("Reader/QCF", qcfV);
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
  m_settings->setValue("Reader/AdaptiveFont", on);
}

void
SettingsDialog::updateQuranFontSize(QString size)
{
  m_settings->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size", size);
  emit quranFontChanged();
  m_renderQuranPage = true;
}

void
SettingsDialog::updateSideFont(QFont fnt)
{
  fnt.setPointSize(m_sideFont.pointSize());
  m_sideFont = fnt;

  m_settings->setValue("Reader/SideContentFont", m_sideFont);
  emit sideFontChanged();
  m_renderSideContent = true;
}

void
SettingsDialog::updateSideFontSize(QString size)
{
  m_sideFont.setPointSize(size.toInt());
  m_settings->setValue("Reader/SideContentFont", m_sideFont);
  emit sideFontChanged();
  m_renderSideContent = true;
}

void
SettingsDialog::applyAllChanges()
{
  QLocale::Language chosenLang =
    qvariant_cast<QLocale::Language>(ui->cmbLang->currentData());
  if (chosenLang != m_languageCode) {
    updateLang(chosenLang);
  }

  if (ui->cmbTheme->currentIndex() != m_themeIdx)
    updateTheme(ui->cmbTheme->currentIndex());

  if (ui->chkDailyVerse->isChecked() != m_votd)
    updateDailyVerse(ui->chkDailyVerse->isChecked());

  if (ui->chkMissingWarning->isChecked() != m_missingFileWarning)
    updateFileWarning(ui->chkMissingWarning->isChecked());

  if (ui->cmbTafsir->currentIndex() != m_tafsir)
    updateTafsir(ui->cmbTafsir->currentIndex());

  if (ui->cmbTranslation->currentIndex() != m_trans)
    updateTranslation(ui->cmbTranslation->currentIndex());

  if (ui->cmbQCF->currentIndex() + 1 != m_qcfVer)
    updateQuranFont(ui->cmbQCF->currentIndex() + 1);

  if (ui->chkAdaptive->isChecked() != m_adaptive)
    updateAdaptiveFont(ui->chkAdaptive->isChecked());

  if (ui->cmbQuranFontSz->currentText() != QString::number(m_quranFontSize))
    updateQuranFontSize(ui->cmbQuranFontSz->currentText());

  if (ui->fntCmbSide->currentFont() != m_sideFont)
    updateSideFont(ui->fntCmbSide->currentFont());

  if (ui->cmbSideFontSz->currentText() !=
      QString::number(m_sideFont.pointSize()))
    updateSideFontSize(ui->cmbSideFontSz->currentText());

  if (ui->cmbAudioDevices->currentIndex() != m_audioOutIdx) {
    m_audioOutIdx = ui->cmbAudioDevices->currentIndex();
    ui->cmbAudioDevices->setCurrentText(
      m_audioDevices.at(m_audioOutIdx).description());
    emit usedAudioDeviceChanged(m_audioDevices.at(m_audioOutIdx));
  }

  // restart after applying all changes if required
  if (m_restartReq) {
    emit restartApp();
    return;
  }

  // redraw once if flag is set
  if (m_renderQuranPage)
    emit redrawQuranPage(false);

  if (m_renderSideContent)
    emit redrawSideContent();

  m_renderQuranPage = false;
  m_renderSideContent = false;
  setCurrentSettingsAsRef();
}

void
SettingsDialog::btnBoxAction(QAbstractButton* btn)
{
  if (btn->text().contains(tr("Apply"))) {
    applyAllChanges();
  } else if (btn->text().contains(tr("Cancel"))) {
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
