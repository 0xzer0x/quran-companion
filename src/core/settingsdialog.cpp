#include "settingsdialog.h"
#include "ui_settingsdialog.h"

/*!
 * \brief SettingsDialog::SettingsDialog class constructor
 *
 * @param parent pointer to parent widget
 * @param settingsPtr pointer to application settings
 * @param vPlayerPtr pointer to The VersePlayer object
 */
SettingsDialog::SettingsDialog(QWidget* parent,
                               QSettings* settingsPtr,
                               VersePlayer* vPlayerPtr,
                               const QString& iconsPath)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , m_resourcePath{ iconsPath }
  , m_settingsPtr{ settingsPtr }
  , m_vPlayerPtr{ vPlayerPtr }
{
  ui->setupUi(this);
  ui->cmbQuranFontSz->setValidator(new QIntValidator(10, 100));
  ui->cmbSideFontSz->setValidator(new QIntValidator(10, 100));
  setWindowIcon(QIcon(m_resourcePath + "/icons/prefs.png"));
  fillLanguageCombobox();

  setCurrentSettingsAsRef();
  ui->cmbLang->setCurrentIndex(ui->cmbLang->findData(m_lang));
  ui->cmbTheme->setCurrentIndex(m_themeIdx);
  ui->cmbQCF->setCurrentIndex(m_qcfVer - 1);
  ui->cmbQuranFontSz->setCurrentText(QString::number(m_quranFontSize));
  ui->fntCmbSide->setCurrentFont(m_sideFont);
  ui->cmbSideFontSz->setCurrentText(QString::number(m_sideFont.pointSize()));
  ui->cmbTafsir->setCurrentIndex(m_tafsir);
  ui->cmbTranslation->setCurrentIndex(m_trans);
  ui->cmbAudioDevices->setCurrentIndex(m_audioOutIdx);
  setRadios();

  // connectors
  setupConnections();
}

void
SettingsDialog::setRadios()
{
  ui->chkDailyVerse->setChecked(m_votd);
  ui->chkMissingWarning->setChecked(m_missingFileWarning);
  ui->chkAdaptive->setChecked(m_adaptive);
}

/*!
 * \brief SettingsDialog::updateTheme slot to update the theme in the settings
 * file & ask user to restart.
 *
 * @param themeIdx
 */
void
SettingsDialog::updateTheme(int themeIdx)
{
  m_settingsPtr->setValue("Theme", themeIdx);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn =
    QMessageBox::question(this,
                          tr("Restart required"),
                          tr("Application theme was changed, restart now?"));

  if (btn == QMessageBox::Yes) {
    m_restartReq = true;
  }
}

/*!
 * \brief SettingsDialog::updateLang slot to update the app language in the
 * settings file & ask user to restart
 * @param lang
 */
void
SettingsDialog::updateLang(QLocale::Language lang)
{
  m_settingsPtr->setValue("Language", lang);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn =
    QMessageBox::question(this,
                          tr("Restart required"),
                          tr("Application language was changed, restart now?"));

  if (btn == QMessageBox::Yes) {
    m_restartReq = true;
  }
}

void
SettingsDialog::updateDailyVerse(bool on)
{
  m_settingsPtr->setValue("VOTD", on);
}

void
SettingsDialog::updateFileWarning(bool on)
{
  m_settingsPtr->setValue("MissingFileWarning", on);
}

/*!
 * \brief SettingsDialog::updateTafsir slot to update the tafsir chosen in the
 * settings file
 * @param idx
 */
void
SettingsDialog::updateTafsir(int idx)
{
  m_settingsPtr->setValue("Reader/Tafsir", idx);
  emit tafsirChanged();
}

/*!
 * \brief SettingsDialog::updateTranslation slot to update the translation
 * chosen in the settings file.
 *
 * @param idx
 */
void
SettingsDialog::updateTranslation(int idx)
{
  m_settingsPtr->setValue("Reader/Translation", idx);
  emit translationChanged();

  m_renderSideContent = true;
}

void
SettingsDialog::updateQuranFont(int qcfV)
{
  m_settingsPtr->setValue("Reader/QCF", qcfV);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn = QMessageBox::question(
    this,
    tr("Restart required"),
    tr("Restart is required to load new quran font, restart now?"));

  if (btn == QMessageBox::Yes) {
    m_restartReq = true;
  }
}

void
SettingsDialog::updateAdaptiveFont(bool on)
{
  m_settingsPtr->setValue("Reader/AdaptiveFont", on);
}

/*!
 * \brief SettingsDialog::updateQuranFontSize slot to update Quran page font
 * size in the settings file
 *
 * @param size
 */
void
SettingsDialog::updateQuranFontSize(QString size)
{
  m_settingsPtr->setValue("Reader/QCF" + QString::number(m_qcfVer) + "Size",
                          size);
  emit quranFontChanged();
  m_renderQuranPage = true;
}

/*!
 * \brief SettingsDialog::updateSideFont slot to update the side font in the
 * settings file
 *
 * @param fnt newly selected font
 */
void
SettingsDialog::updateSideFont(QFont fnt)
{
  fnt.setPointSize(m_sideFont.pointSize());
  m_sideFont = fnt;

  m_settingsPtr->setValue("Reader/SideContentFont", m_sideFont);
  emit sideFontChanged();
  m_renderSideContent = true;
}

/*!
 * \brief SettingsDialog::updateSideFontSize slot to update the side font size
 * in the settings file
 *
 * @param size
 */
void
SettingsDialog::updateSideFontSize(QString size)
{
  m_sideFont.setPointSize(size.toInt());
  m_settingsPtr->setValue("Reader/SideContentFont", m_sideFont);
  emit sideFontChanged();
  m_renderSideContent = true;
}

/*!
 * \brief SettingsDialog::applyAllChanges slot to check whether any value of the
 * settings in the dialog changed and apply it
 */
void
SettingsDialog::applyAllChanges()
{

  QLocale::Language chosenLang =
    qvariant_cast<QLocale::Language>(ui->cmbLang->currentData());
  if (chosenLang != m_lang) {
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
SettingsDialog::showWindow()
{
  setCurrentSettingsAsRef();
  ui->cmbLang->setCurrentIndex(ui->cmbLang->findData(m_lang));
  ui->cmbTheme->setCurrentIndex(m_themeIdx);
  ui->cmbQCF->setCurrentIndex(m_qcfVer - 1);
  ui->cmbQuranFontSz->setCurrentText(QString::number(m_quranFontSize));
  ui->fntCmbSide->setCurrentFont(m_sideFont);
  ui->cmbSideFontSz->setCurrentText(QString::number(m_sideFont.pointSize()));
  ui->cmbTafsir->setCurrentIndex(m_tafsir);
  ui->cmbTranslation->setCurrentIndex(m_trans);
  ui->cmbAudioDevices->setCurrentIndex(m_audioOutIdx);
  setRadios();

  this->show();
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

/*!
 * \brief SettingsDialog::setCurrentSettingsAsRef slot to update the settings
 * displayed to match the values in the settings file
 */
void
SettingsDialog::setCurrentSettingsAsRef()
{
  m_themeIdx = m_settingsPtr->value("Theme").toInt();
  m_lang = qvariant_cast<QLocale::Language>(m_settingsPtr->value("Language"));
  m_votd = m_settingsPtr->value("VOTD").toBool();
  m_missingFileWarning = m_settingsPtr->value("MissingFileWarning").toBool();

  m_settingsPtr->beginGroup("Reader");
  // all keys have prefix "Reader"
  m_qcfVer = m_settingsPtr->value("QCF").toInt();
  m_adaptive = m_settingsPtr->value("AdaptiveFont").toBool();
  m_quranFontSize =
    m_settingsPtr->value("QCF" + QString::number(m_qcfVer) + "Size").toInt();
  m_sideFont = qvariant_cast<QFont>(m_settingsPtr->value("SideContentFont"));
  m_tafsir = m_settingsPtr->value("Tafsir").toInt();
  m_trans = m_settingsPtr->value("Translation").toInt();
  m_settingsPtr->endGroup();

  m_audioDevices = QMediaDevices::audioOutputs();
  ui->cmbAudioDevices->clear();
  for (int i = 0; i < m_audioDevices.length(); i++) {
    ui->cmbAudioDevices->addItem(m_audioDevices.at(i).description());

    if (m_audioDevices.at(i) == m_vPlayerPtr->getOutput()->device())
      m_audioOutIdx = i;
  }
  ui->cmbAudioDevices->setCurrentIndex(m_audioOutIdx);
}

/*!
 * \brief SettingsDialog::btnBoxAction slot takes action according to the button
 * clicked in the dialog
 *
 * @param btn
 */
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
SettingsDialog::closeEvent(QCloseEvent* event)
{
  this->hide();
}

SettingsDialog::~SettingsDialog()
{
  delete ui;
}
