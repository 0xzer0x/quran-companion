/**
 * @file settingsdialog.h
 * @brief Header file for SettingsDialog
 */

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "../utils/verseplayer.h"
#include <QApplication>
#include <QAudioDevice>
#include <QColorDialog>
#include <QDialog>
#include <QIntValidator>
#include <QKeySequence>
#include <QKeySequenceEdit>
#include <QMediaDevices>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QShortcut>
#include <QStandardItemModel>
#include <QValidator>

namespace Ui {
class SettingsDialog;
}

/**
 * @brief SettingsDialog class represents a UI for manipulating application
 * settings.
 */
class SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   * @param vPlayerPtr - pointer to VersePlayer instance used for playback
   */
  explicit SettingsDialog(QWidget* parent = nullptr,
                          VersePlayer* vPlayerPtr = nullptr);
  ~SettingsDialog();

public slots:
  /**
   * @brief Slot thats called when one of the 3 buttons at the bottom of the
   * dialog is clicked (Apply - Ok - Cancel).
   * @param btn - pointer to the clicked button widget
   */
  void btnBoxAction(QAbstractButton* btn);
  /**
   * @brief Slot to update the theme in the settings file & set the restart
   * flag.
   * @param themeIdx - theme index in the combobox (0 - light, 1 - dark)
   */
  void updateTheme(int themeIdx);
  /**
   * @brief Slot to update the app language in the settings file & set the
   * restart flag.
   * @param lang - QLocale::Language enum value representing the new language
   */
  void updateLang(QLocale::Language lang);
  /**
   * @brief Update daily verse option
   * @param on - boolean flag representing the new setting value
   */
  void updateDailyVerse(bool on);
  /**
   * @brief Update the missing recitation warning
   * @param on - boolean flag representing the new setting value
   */
  void updateFileWarning(bool on);
  /**
   * @brief Update the selected tafsir
   * @param idx - index of the tafsir value in DBManager::Tafsir enum
   */
  void updateTafsir(int idx);
  /**
   * @brief Update the selected translation
   * @param idx - index of the translation value in DBManager::Translation enum
   */
  void updateTranslation(int idx);
  /**
   * @brief update the ::ReaderMode used
   * @param idx - index of the new ::ReaderMode
   */
  void updateReaderMode(int idx);
  /**
   * @brief Update the QCF font used
   * @param qcfV - qcf version to change to
   */
  void updateQuranFont(int qcfV);
  /**
   * @brief Update the state for Adaptive page font size feature
   * @param on - boolean flag representing the new setting value
   */
  void updateAdaptiveFont(bool on);
  /**
   * @brief Update set the new font size for the used QCF font
   * @param size - QString representing the new font size
   */
  void updateQuranFontSize(QString size);
  /**
   * @brief update the boolean indicating foreground highlighting
   * @param on - boolean flag indicating whether to change the foreground color
   * of the current verse
   */
  void updateFgHighlight(bool on);
  /**
   * @brief Update the font used for displaying the translation in the side
   * panel
   * @param fnt - QFont representing the new font
   */
  void updateSideFont(QFont fnt);
  /**
   * @brief Update the font size for the translation/side panel font
   * @param size - QString representing the new font size
   */
  void updateSideFontSize(QString size);
  void updateVerseText(int vt);
  void updateVerseTextFontsize(QString size);
  /**
   * @brief update the key sequence that trigger the shortcut with the given key
   * @param key - QString of the shortcut name in the settings file
   * @param keySequence - QString representation of the key sequence
   */
  void updateShortcut(QString key, QString keySequence);
  /**
   * @brief check for changes in all settings and apply new settings if changes
   * are found.
   */
  void applyAllChanges();
  /**
   * @brief Set the current UI elements to match the entries in the settings
   * file before showing the dialog.
   */
  void showWindow();

signals:
  /**
   * MODIFIED
   */
  void qcf2Missing();
  /**
   * @fn quranFontChanged()
   * @brief signal emitted for QuranPageBrowser to set the new Quran page font
   * size.
   */
  void quranFontChanged();
  /**
   * @fn highlightLayerChanged()
   * @brief signal emitted in order to change the layer to which highlighting is
   * applied.
   */
  void highlightLayerChanged();
  /**
   * @fn redrawSideContent()
   * @brief signal emitted to reload the verse-by-verse side panel to match the
   * newly set side font/translation.
   */
  void redrawSideContent();
  /**
   * @fn redrawQuranPage(bool)
   * @brief signal emitted to reload the Quran page.
   * @param manual - boolean flag to force use the manually set font size during
   * Quran page construction.
   */
  void redrawQuranPage(bool manual);
  /**
   * @fn tafsirChanged()
   * @brief signal emitted to notify changes in the displayed tafsir
   */
  void tafsirChanged();
  /**
   * @fn translationChanged()
   * @brief signal emitted to notify changes in the displayed translation
   */
  void translationChanged();
  /**
   * @fn sideFontChanged()
   * @brief signal emitted to notify changes in the side panel font.
   */
  void sideFontChanged();
  /**
   * MODIFIED
   */
  void verseTypeChanged();
  /**
   * @fn usedAudioDeviceChanged()
   * @brief signal emitted in order to changed audio output device used by
   * VersePlayer for playback.
   * @param dev - QAudioDevice instance for the selected device
   */
  void usedAudioDeviceChanged(QAudioDevice dev);
  /**
   * @fn shortcutChanged()
   * @brief signal emitted in order to notify a change in the key sequence of
   * shortcut
   * @param key - the key of the changed shortcut
   */
  void shortcutChanged(QString key);
  /**
   * @fn restartApp()
   * @brief signal emitted for changes that require restart to take place.
   */
  void restartApp();

protected:
  /** @brief Re-implementation of QWidget::closeEvent() in order to hide the
   * window instead of closing it.
   * @param event
   */
  void closeEvent(QCloseEvent* event);

private:
  const int m_qcfVer = Globals::qcfVersion;
  const int m_themeIdx = Globals::themeId;
  const ReaderMode m_readerMode = Globals::readerMode;
  const QLocale::Language m_languageCode = Globals::language;
  QSettings* const m_settings = Globals::settings;
  const QDir& m_downloadsDir = Globals::downloadsDir;
  const QList<Tafsir>& m_tafasirList = Globals::tafasirList;
  const QList<Translation>& m_translationsList = Globals::translationsList;
  const QMap<QString, QString>& m_shortcutDescription =
    Globals::shortcutDescription;
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief generate m_shortcutsModel from the settings group
   */
  void populateShortcutsModel();
  /**
   * @brief adds all supported language entries in the langauge combobox.
   */
  void fillLanguageCombobox();
  /**
   * MODIFIED
   */
  void fillContentCombobox();
  /**
   * @brief loads all the settings from the settings file into their
   * corresponding variables and updates UI components to match them.
   */
  void setCurrentSettingsAsRef();
  /**
   * @brief check if any shortcut was changed and updated it
   */
  void checkShortcuts();
  /**
   * MODIFIED
   */
  bool qcfExists();
  /**
   * MODIFIED
   */
  bool tafsirExists(int idx);
  /**
   * @brief QCF font size used in constructing Quran page.
   */
  int m_quranFontSize;
  /**
   * @brief index of currently in-use audio output device in
   * SettingsDialog::m_audioDevices.
   */
  int m_audioOutIdx;
  /**
   * @brief DBManager::Tafsir enum value mapped to the tafsir index in the
   * combobox.
   */
  int m_tafsir;
  /**
   * @brief DBManager::Translation enum value mapped to the translation index in
   * the combobox.
   */
  int m_translation;
  int m_verseType;
  int m_verseFontSize;
  /**
   * @brief boolean flag representing the verse of the day option checkbox
   * state.
   */
  bool m_votd = true;
  /**
   * @brief boolean flag representing the adaptive page font option checkbox
   * state.
   */
  bool m_adaptive = true;
  /**
   * @brief boolean flag representing the missing recitation warning option
   * checkbox state.
   */
  bool m_missingFileWarning = true;
  /**
   * @brief boolean flag set when side panel settings are changed in order to
   * notify the MainWindow to reload the side content.
   */
  bool m_renderSideContent = false;
  /**
   * @brief boolean flag set when QCF font size is changed in order to
   * notify the MainWindow to reload the Quran page.
   */
  bool m_renderQuranPage = false;
  /**
   * @brief boolean flag set in order to notify the app that a restart is
   * required.
   */
  bool m_restartReq = false;
  /**
   * @brief boolean flag to indicate shortcut conflict checking
   */
  bool m_checkingShortcut = false;
  /**
   * @brief boolean flag to indicate that foreground highlighting is active.
   */
  bool m_fgHighlight = true;
  /**
   * @brief Pointer to access ui elements generated from .ui files.
   */
  Ui::SettingsDialog* ui;
  /**
   * @brief pointer to VersePlayer instance.
   */
  VersePlayer* m_vPlayerPtr = nullptr;
  /**
   * @brief model used by the shortcuts QTableView
   */
  QStandardItemModel m_shortcutsModel;
  /**
   * @brief QList for all available audio output devices on the system.
   */
  QList<QAudioDevice> m_audioDevices;
  /**
   * @brief font used for displaying translation/tafsir in the application.
   */
  QFont m_sideFont;
};

#endif // SETTINGSDIALOG_H
