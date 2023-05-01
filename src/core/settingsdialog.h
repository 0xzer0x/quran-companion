#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "../utils/verseplayer.h"
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

namespace Ui {
class SettingsDialog;
}

/*!
 * \class SettingsDialog
 *
 * \brief The SettingsDialog class represents a UI for manipulating application
 * settings
 */
class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SettingsDialog(QWidget* parent = nullptr,
                          QSettings* settingsPtr = nullptr,
                          VersePlayer* vPlayerPtr = nullptr,
                          const QString& iconsPath = ":/images/light/");
  ~SettingsDialog();

public slots:
  void btnBoxAction(QAbstractButton* btn);
  void updateTheme(int themeIdx);
  void updateLang(QLocale::Language lang);
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
  Ui::SettingsDialog* ui;
  QString m_iconsPath;
  QSettings* m_settingsPtr;
  QLocale::Language m_lang;
  QList<QAudioDevice> m_audioDevices;
  VersePlayer* m_vPlayerPtr;
  QFont m_sideFont;
  int m_themeIdx;
  int m_qcfVer;
  int m_quranFontSize;
  int m_audioOutIdx;
  int m_sideContent;
  int m_tafsir;
  int m_trans;
  bool m_renderSideContent = false;
  bool m_renderQuranPage = false;
  bool m_restartReq = false;
  void fillLanguageCombobox();
  void setCurrentSettingsAsRef();

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);
};

#endif // SETTINGSDIALOG_H
