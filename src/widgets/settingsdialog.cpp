#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent, QSettings *settingsPtr, VersePlayer *vPlayerPtr)
    : QDialog(parent)
    , ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/assets/images/prefs.png"));
    m_settingsPtr = settingsPtr;
    m_vPlayerPtr = vPlayerPtr;

    if (m_settingsPtr->value("Language").toString() == "العربية") {
        ui->retranslateUi(this);
    }
    setWindowTitle(tr("Preferences"));
    setCurrentSettingsAsRef();

    ui->cmbTheme->setCurrentIndex(m_themeIdx);
    ui->cmbLang->setCurrentText(m_lang);

    ui->cmbQuranFontSz->setCurrentText(m_quranFontSize);
    ui->chkCopyVerseOnClick->setChecked(m_cpyVerseChk);
    ui->fntCmbSide->setCurrentFont(m_sideFont);
    ui->cmbSideFontSz->setCurrentText(QString::number(m_sideFont.pointSize()));
    ui->cmbSideContent->setCurrentIndex(m_sideContent);
    ui->cmbTafsir->setCurrentIndex(m_tafsir);
    ui->cmbTranslation->setCurrentIndex(m_trans);

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &SettingsDialog::btnBoxAction);
}

void SettingsDialog::updateTheme(int themeIdx)
{
    m_settingsPtr->setValue("Theme", themeIdx);
    QMessageBox::StandardButton btn
        = QMessageBox::question(this,
                                tr("Restart required"),
                                tr("Application theme was changed, restart now?"));

    if (btn == QMessageBox::Yes) {
        emit QApplication::exit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

void SettingsDialog::updateLang(QString langName)
{
    m_settingsPtr->setValue("Language", langName);

    QMessageBox::StandardButton btn
        = QMessageBox::question(this,
                                tr("Restart required"),
                                tr("Application language was changed, restart now?"));

    if (btn == QMessageBox::Yes) {
        emit QApplication::exit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

void SettingsDialog::updateSideContent(int idx)
{
    m_settingsPtr->setValue("Reader/SideContent", idx);
    emit sideContentTypeChanged();

    m_renderSideContent = true;
}

void SettingsDialog::updateTafsir(int idx)
{
    m_settingsPtr->setValue("Reader/Tafsir", idx);
    emit tafsirChanged();

    if (m_sideContent == 0)
        m_renderSideContent = true;
}

void SettingsDialog::updateTranslation(int idx)
{
    m_settingsPtr->setValue("Reader/Translation", idx);
    emit translationChanged();

    if (m_sideContent == 1)
        m_renderSideContent = true;
}

void SettingsDialog::updateQuranFontSize(QString size)
{
    m_settingsPtr->setValue("Reader/QuranFontSize", size);
    emit quranFontChanged();
    m_renderQuranPage = true;
}

void SettingsDialog::updateSideFont(QFont fnt)
{
    fnt.setPointSize(m_sideFont.pointSize());
    m_sideFont = fnt;

    m_settingsPtr->setValue("Reader/SideContentFont", m_sideFont);
    emit sideFontChanged();
    m_renderSideContent = true;
}

void SettingsDialog::updateSideFontSize(QString size)
{
    m_sideFont.setPointSize(size.toInt());
    m_settingsPtr->setValue("Reader/SideContentFont", m_sideFont);
    emit sideFontChanged();
    m_renderSideContent = true;
}

void SettingsDialog::updateCpyVerseChk(bool state)
{
    m_settingsPtr->setValue("Reader/CopyVerseOnClick", state);
}

void SettingsDialog::applyAllChanges()
{
    if (ui->cmbTheme->currentIndex() != m_themeIdx) {
        updateTheme(ui->cmbTheme->currentIndex());
    }

    if (ui->cmbLang->currentText() != m_lang) {
        updateLang(ui->cmbLang->currentText());
    }

    if (ui->cmbSideContent->currentIndex() != m_sideContent) {
        updateSideContent(ui->cmbSideContent->currentIndex());
    }

    if (ui->cmbTafsir->currentIndex() != m_tafsir) {
        updateTafsir(ui->cmbTafsir->currentIndex());
    }

    if (ui->cmbTranslation->currentIndex() != m_trans) {
        updateTranslation(ui->cmbTranslation->currentIndex());
    }

    if (ui->cmbQuranFontSz->currentText() != m_quranFontSize) {
        updateQuranFontSize(ui->cmbQuranFontSz->currentText());
    }

    if (ui->fntCmbSide->currentFont() != m_sideFont) {
        updateSideFont(ui->fntCmbSide->currentFont());
    }

    if (ui->cmbSideFontSz->currentText() != QString::number(m_sideFont.pointSize())) {
        updateSideFontSize(ui->cmbSideFontSz->currentText());
    }

    if (ui->chkCopyVerseOnClick->isChecked() != m_cpyVerseChk) {
        updateCpyVerseChk(ui->chkCopyVerseOnClick->isChecked());
    }

    if (ui->cmbAudioDevices->currentIndex() != m_audioOutIdx) {
        m_audioOutIdx = ui->cmbAudioDevices->currentIndex();
        emit usedAudioDeviceChanged(m_audioDevices.at(m_audioOutIdx));
    }

    // redraw once if flag is set
    if (m_renderQuranPage)
        emit redrawQuranPage();

    if (m_renderSideContent)
        emit redrawSideContent();

    m_renderQuranPage = false;
    m_renderSideContent = false;
    setCurrentSettingsAsRef();
}

void SettingsDialog::setCurrentSettingsAsRef()
{
    m_themeIdx = m_settingsPtr->value("Theme").toInt();
    m_lang = m_settingsPtr->value("Language").toString();

    m_settingsPtr->beginGroup("Reader");

    m_quranFontSize = m_settingsPtr->value("QuranFontSize").toString();
    m_sideFont = qvariant_cast<QFont>(m_settingsPtr->value("SideContentFont"));
    m_sideContent = m_settingsPtr->value("SideContent").toInt();
    m_tafsir = m_settingsPtr->value("Tafsir").toInt();
    m_trans = m_settingsPtr->value("Translation").toInt();
    m_cpyVerseChk = m_settingsPtr->value("CopyVerseOnClick").toBool();
    m_highlightClr = m_settingsPtr->value("HighlightColor").toString();

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

void SettingsDialog::btnBoxAction(QAbstractButton *btn)
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

SettingsDialog::~SettingsDialog()
{
    delete ui;
}
