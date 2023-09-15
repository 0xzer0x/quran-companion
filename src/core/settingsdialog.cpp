/**
 * @file settingsdialog.cpp
 * @brief Implementation file for SettingsDialog
 */

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget* parent, VersePlayer* vPlayerPtr)
  : QDialog(parent)
  , ui(new Ui::SettingsDialog)
  , m_vPlayerPtr(vPlayerPtr)
{
  ui->setupUi(this);
  ui->cmbQuranFontSz->setValidator(new QIntValidator(10, 100));
  ui->cmbSideFontSz->setValidator(new QIntValidator(10, 100));
  setWindowIcon(QIcon(m_resources.filePath("icons/prefs.png")));
  fillLanguageCombobox();
  ui->tableViewShortcuts->setModel(&m_shortcutsModel);
  ui->tableViewShortcuts->horizontalHeader()->setStretchLastSection(true);
  m_keySeqEdit = new QKeySequenceEdit(this);
  m_keySeqEdit->hide();

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
  connect(ui->tableViewShortcuts,
          &QTableView::doubleClicked,
          this,
          &SettingsDialog::editShortcut,
          Qt::UniqueConnection);
  connect(m_keySeqEdit,
          &QKeySequenceEdit::editingFinished,
          this,
          &SettingsDialog::setShortcut,
          Qt::UniqueConnection);
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
    keySeq->setText(m_settings->value("Shortcuts/" + key).toString());

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
}

void
SettingsDialog::setCurrentSettingsAsRef()
{
  m_votd = m_settings->value("VOTD").toBool();
  m_missingFileWarning = m_settings->value("MissingFileWarning").toBool();

  m_adaptive = m_settings->value("Reader/AdaptiveFont").toBool();
  m_quranFontSize =
    m_settings->value("Reader/QCF" + QString::number(m_qcfVer) + "Size")
      .toInt();
  m_sideFont =
    qvariant_cast<QFont>(m_settings->value("Reader/SideContentFont"));
  m_tafsir = m_settings->value("Reader/Tafsir").toInt();
  m_translation = m_settings->value("Reader/Translation").toInt();

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
  ui->cmbReaderMode->setCurrentIndex(m_readerMode);
  ui->cmbQCF->setCurrentIndex(m_qcfVer - 1);
  ui->cmbQuranFontSz->setCurrentText(QString::number(m_quranFontSize));
  ui->fntCmbSide->setCurrentFont(m_sideFont);
  ui->cmbSideFontSz->setCurrentText(QString::number(m_sideFont.pointSize()));
  ui->cmbTafsir->setCurrentIndex(m_tafsir);
  ui->cmbTranslation->setCurrentIndex(m_translation);
  ui->cmbAudioDevices->setCurrentIndex(m_audioOutIdx);
  ui->chkDailyVerse->setChecked(m_votd);
  ui->chkAdaptive->setChecked(m_adaptive);
  ui->chkMissingWarning->setChecked(m_missingFileWarning);

  // shortcuts tab
  populateShortcutsModel();
  ui->tableViewShortcuts->resizeColumnsToContents();
}

bool
SettingsDialog::shortcutAvailable(QString keySequence)
{
  // check if any item in the shortcut model contains the same key sequence
  bool available =
    m_shortcutsModel.findItems(keySequence, Qt::MatchExactly, 1).empty();

  return available;
}

void
SettingsDialog::checkShortcuts()
{
  for (int row = 0; row < m_shortcutsModel.rowCount(); row++) {
    const QString& key =
      m_shortcutsModel.item(row, 0)->data(Qt::UserRole).toString();
    const QString& keySequence = m_shortcutsModel.item(row, 1)->text();
    if (m_settings->value("Shortcuts/" + key).toString() != keySequence)
      updateShortcut(key, keySequence);
  }
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
SettingsDialog::updateReaderMode(int idx)
{
  m_settings->setValue("Reader/Mode", idx);
  if (m_restartReq)
    return;

  QMessageBox::StandardButton btn = QMessageBox::question(
    this, tr("Restart required"), tr("Reading mode was changed, restart now?"));

  m_restartReq = btn == QMessageBox::Yes;
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
SettingsDialog::updateShortcut(QString key, QString keySequence)
{
  m_settings->setValue("Shortcuts/" + key, keySequence);
  emit shortcutChanged(key);
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

  if (ui->cmbTranslation->currentIndex() != m_translation)
    updateTranslation(ui->cmbTranslation->currentIndex());

  if (ui->cmbQCF->currentIndex() + 1 != m_qcfVer)
    updateQuranFont(ui->cmbQCF->currentIndex() + 1);

  if (ui->chkAdaptive->isChecked() != m_adaptive)
    updateAdaptiveFont(ui->chkAdaptive->isChecked());

  if (ui->cmbReaderMode->currentIndex() != m_readerMode)
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

  if (ui->cmbAudioDevices->currentIndex() != m_audioOutIdx) {
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
SettingsDialog::editShortcut(const QModelIndex& index)
{
  QString key =
    m_shortcutsModel.index(index.row(), 0).data(Qt::UserRole).toString();

  m_keySeqEdit->setObjectName(key + "-" + QString::number(index.row()));
  m_keySeqEdit->resize(m_keySeqEdit->width() + 20, m_keySeqEdit->height() + 20);
  m_keySeqEdit->move((width() - m_keySeqEdit->width()) / 2,
                     (height() - m_keySeqEdit->height()) / 2);

  m_keySeqEdit->setKeySequence(m_shortcutsModel.item(index.row(), 1)->text());
  m_keySeqEdit->show();
  m_keySeqEdit->setFocus();
}

void
SettingsDialog::setShortcut()
{
  if (m_checkingShortcut)
    return;

  m_checkingShortcut = true;
  QStringList key = m_keySeqEdit->objectName().split('-');
  QString value = m_keySeqEdit->keySequence().toString();

  m_keySeqEdit->hide();
  qInfo() << "set shortcut triggered for " + key.at(0) + ": " + value;

  if (shortcutAvailable(value))
    m_shortcutsModel.item(key.at(1).toInt(), 1)->setText(value);
  else {
    QMessageBox::warning(this,
                         tr("Shortcut conflicts"),
                         tr("This key combination is used by another "
                            "shortcut, please use a different one."));
  }

  m_checkingShortcut = false;
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
  m_keySeqEdit->hide();
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
