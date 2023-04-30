#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QApplication>
#include <QAudioDevice>
#include <QColorDialog>
#include <QDialog>
#include <QIntValidator>
#include <QMediaDevices>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QValidator>
#include "../utils/verseplayer.h"

namespace Ui {
class SettingsDialog;
}

/*!
 * \class SettingsDialog
 * 
 * \brief The SettingsDialog class represents a UI for manipulating application settings
 */
class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr,
                            QSettings *settingsPtr = nullptr,
                            VersePlayer *vPlayerPtr = nullptr,
                            const QString &iconsPath = ":/images/light/");
    ~SettingsDialog();

public slots:
    void btnBoxAction(QAbstractButton *btn);
    void updateTheme(int themeIdx);
    void updateLang(QString langName);
    void updateSideContent(int idx);
    void updateTafsir(int idx);
    void updateTranslation(int idx);
    void updateQuranFont(int qcfV);
    void updateQuranFontSize(QString size);
    void updateSideFont(QFont fnt);
    void updateSideFontSize(QString size);
    void applyAllChanges();
    void showWindow();

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
    void restartApp();

private:
    Ui::SettingsDialog *ui;
    QString m_iconsPath;
    QSettings *m_settingsPtr;
    int m_themeIdx;
    QString m_lang;
    int m_qcfVer;
    int m_quranFontSize;
    QList<QAudioDevice> m_audioDevices;
    VersePlayer *m_vPlayerPtr;
    int m_audioOutIdx;
    int m_sideContent;
    int m_tafsir;
    int m_trans;
    QFont m_sideFont;
    bool m_renderSideContent = false;
    bool m_renderQuranPage = false;
    bool m_restartReq = false;
    void setCurrentSettingsAsRef();

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SETTINGSDIALOG_H
