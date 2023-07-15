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
                          VersePlayer* vPlayerPtr = nullptr);
  ~SettingsDialog();

public slots:
  void btnBoxAction(QAbstractButton* btn);
  void updateTheme(int themeIdx);
  void updateLang(QLocale::Language lang);
  void updateDailyVerse(bool on);
  void updateFileWarning(bool on);
  void updateTafsir(int idx);
  void updateTranslation(int idx);
  void updateQuranFont(int qcfV);
  void updateAdaptiveFont(bool on);
  void updateQuranFontSize(QString size);
  void updateSideFont(QFont fnt);
  void updateSideFontSize(QString size);
  void applyAllChanges();
  void showWindow();

signals:
  void quranFontChanged();
  void highlightColorChanged();
  void redrawSideContent();
  void redrawQuranPage(bool manual);
  void tafsirChanged();
  void translationChanged();
  void sideFontChanged();
  void usedAudioDeviceChanged(QAudioDevice dev);
  void restartApp();

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);

private:
  QSettings* const m_settings = g_settings;
  const QDir& m_resources = g_themeResources;
  void setupConnections();
  void fillLanguageCombobox();
  void setCurrentSettingsAsRef();
  int m_themeIdx;
  int m_qcfVer;
  int m_quranFontSize;
  int m_audioOutIdx;
  int m_tafsir;
  int m_trans;
  bool m_votd = true;
  bool m_adaptive = true;
  bool m_missingFileWarning = true;
  bool m_renderSideContent = false;
  bool m_renderQuranPage = false;
  bool m_restartReq = false;
  Ui::SettingsDialog* ui;
  VersePlayer* m_vPlayerPtr;
  QList<QAudioDevice> m_audioDevices;
  QLocale::Language m_languageCode;
  QFont m_sideFont;
};

#endif // SETTINGSDIALOG_H
