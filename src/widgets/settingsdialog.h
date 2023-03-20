#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QApplication>
#include <QAudioDevice>
#include <QColorDialog>
#include <QDialog>
#include <QMediaDevices>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include "../utils/verseplayer.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr,
                            QSettings *settingsPtr = nullptr,
                            VersePlayer *vPlayerPtr = nullptr);
    ~SettingsDialog();

signals:
    void quranFontChanged();
    void highlightColorChanged();
    void redrawSideContent();
    void redrawQuranPage();
    void sideContentTypeChanged();
    void tafsirChanged();
    void translationChanged();
    void sideFontChanged();
    void usedAudioDeviceChanged(QAudioDevice dev);

public slots:
    void btnBoxAction(QAbstractButton *btn);
    void updateTheme(QString themeName);
    void updateLang(QString langName);
    void updateSideContent(int idx);
    void updateTafsir(int idx);
    void updateTranslation(int idx);
    void updateQuranFontSize(QString size);
    void updateSideFont(QFont fnt);
    void updateSideFontSize(QString size);
    void updateCpyVerseChk(bool state);
    void applyAllChanges();

private:
    Ui::SettingsDialog *ui;
    QSettings *m_settingsPtr;
    QString m_theme;
    QString m_lang;
    QString m_highlightClr;
    QString m_quranFontSize;
    QList<QAudioDevice> m_audioDevices;
    VersePlayer *m_vPlayerPtr;
    int m_audioOutIdx;
    int m_sideContent;
    int m_tafsir;
    int m_trans;
    QFont m_sideFont;
    bool m_cpyVerseChk;
    bool m_renderSideContent = false;
    bool m_renderQuranPage = false;
    void setCurrentSettingsAsRef();
};

#endif // SETTINGSDIALOG_H
