/**
 * @file mainwindow.h
 * @brief Header file for MainWindow
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QBoxLayout>
#include <QIntValidator>
#include <QMainWindow>
#include <QScrollArea>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QStringListModel>
#include <components/playercontrols.h>
#include <components/quranreader.h>
#include <components/systemtray.h>
#include <dialogs/bookmarksdialog.h>
#include <dialogs/contentdialog.h>
#include <dialogs/copydialog.h>
#include <dialogs/downloaderdialog.h>
#include <dialogs/fileselector.h>
#include <dialogs/importexportdialog.h>
#include <dialogs/khatmahdialog.h>
#include <dialogs/searchdialog.h>
#include <dialogs/settingsdialog.h>
#include <dialogs/versedialog.h>
#include <player/verseplayer.h>
#include <service/bookmarkservice.h>
#include <service/quranservice.h>
#include <service/translationservice.h>
#include <types/verse.h>
#include <utils/shortcuthandler.h>
#include <utils/versionchecker.h>
#include <widgets/betaqaviewer.h>
#include <widgets/notificationpopup.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief MainWindow class is the driver class the connects all the other
 * components and is responsible for the navigation dock and menubar
 * functionality
 */
class MainWindow
  : public QMainWindow
  , public VerseObserver
{
  Q_OBJECT

public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   */
  explicit MainWindow(QWidget* parent);
  ~MainWindow();

  void activeVerseChanged();

public slots:
  /**
   * @brief save the current position and window state of the application to the
   * settings file
   */
  void saveReaderState();
  /**
   * @brief restart the application
   */
  void restartApp();

protected:
  /**
   * @brief re-implementation of QWidget::resizeEvent(QResizeEvent*) to resize
   * the NotificationPopup size accordingly
   * @param event
   */
  void resizeEvent(QResizeEvent* event);

private slots:
  /**
   * @brief sets the verse combobox values according to the current surah verse
   * count (m_surahCount), sets the current verse as the visible index
   * @param forceUpdate - boolean flag forces the combobox to update it's surah
   * verse count
   */
  void setVerseComboBoxRange(bool forceUpdate = false);
  /**
   * @brief slot for updating the reader page as the user selects a different
   * page from the combobox
   * @param newPageIdx - the selected idx in the combobox (0 - 603)
   */
  void cmbPageChanged(int newPageIdx);
  /**
   * @brief slot for updating the reader page as the user selects a different
   * verse in the same surah
   * @param newVerseIdx - verse idx in the combobox (0 - (m_surahCount-1))
   */
  void cmbVerseChanged(int newVerseIdx);
  /**
   * @brief slot for updating the reader page as the user selects a different
   * juz in the same surah
   * @param newJuzIdx - juz idx in the combobox (0 - 29)
   */
  void cmbJuzChanged(int newJuzIdx);
  /**
   * @brief display warning message box in case that recitation files are
   * missing
   * @param reciterIdx - ::Reciter::reciters index for the reciter
   * @param surah
   */
  void missingRecitationFileWarn(int reciterIdx, int surah);
  /**
   * @brief display a warning messagebox when QCF2 files are not found
   */
  void missingQCF();
  /**
   * @brief display a warning messagebox when a tafsir db file is not found
   * @param idx - index of tafsir in Tafsir::tafasir
   */
  void missingTafsir(int idx);
  /**
   * @brief display a warning messagebox when a translation db file is not found
   * @param idx - index of translation in Translation::translations
   */
  void missingTranslation(int idx);
  /**
   * @brief set the systray icon tooltip text according to the currently playing
   * recitation
   * @param state - playback state of the current verse
   */
  void updateTrayTooltip(QMediaPlayer::PlaybackState state);
  /**
   * @brief adds the current Verse to the bookmarks
   */
  void bookmarkCurrent();
  /**
   * @brief actionUpdatesTriggered
   */
  void actionUpdatesTriggered();
  /**
   * @brief open the SettingsDialog and connect settings change slots
   */
  void actionPrefTriggered();
  /**
   * @brief open the DownloaderDialog, create instance if not set
   */
  void actionDMTriggered();
  /**
   * @brief open the BookmarksDialog, create instance if not set
   */
  void actionBookmarksTriggered();
  /**
   * @brief open the KhatmahDialog, create instance if not set
   */
  void actionKhatmahTriggered();
  /**
   * @brief open the CopyDialog, create instance if not set
   */
  void actionAdvancedCopyTriggered();
  /**
   * @brief open the ContentDialog for the current Verse
   */
  void actionTafsirTriggered();
  /**
   * @brief open the verse of the day dialog
   */
  void actionVotdTriggered();
  /**
   * @brief open the SearchDialog, create instance if not set
   */
  void actionSearchTriggered();
  /**
   * @brief callback for the checkbox to toggle the visibility of the player
   * controls
   * @param checked - boolean representing check state of ui checkbox
   */
  void actionPlayerControlsToggled(bool checked);
  /**
   * @brief open the about messagebox for the application
   */
  void actionAboutTriggered();
  /**
   * @brief open the about messagebox for Qt
   */
  void actionAboutQttriggered();
  /**
   * @brief move to the next Verse relative to m_currVerse
   */
  void nextVerse();
  /**
   * @brief move to the previous Verse relative to m_currVerse
   */
  void prevVerse();
  /**
   * @brief move to the next Juz
   */
  void nextJuz();
  /**
   * @brief move to the previous Juz
   */
  void prevJuz();
  /**
   * @brief move to the next surah
   */
  void nextSurah();
  /**
   * @brief move to the previous surah
   */
  void prevSurah();
  /**
   * @brief navigate to the surah with the given QModelIndex in the surahs list
   * model
   */
  void navigateToSurah(QModelIndex& index);
  /**
   * @brief search for the surahs with the given argument when the text in the
   * side dock search box is changed
   * @param arg1 - QString of the new text in the search box
   */
  void searchSurahTextChanged(const QString& arg1);
  /**
   * @brief navigate to the surah clicked in the QListView in the side dock,
   * index might differ than the actual surah number
   * @param index - QModelIndex of the clicked surah
   */
  void listSurahNameClicked(const QModelIndex& index);
  /**
   * @brief toggles visiblity of menubar
   */
  void toggleMenubar();
  /**
   * @brief toggles visiblity of the navigation dock
   */
  void toggleNavDock();
  /**
   * @brief callback function for importing user data from file
   */
  void importUserData();
  /**
   * @brief callback function for exporting user data to file
   */
  void exportUserData();

private:
  Ui::MainWindow* ui;
  /**
   * @brief reference to the shared current verse instance
   */
  Verse& m_currVerse;
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;
  /**
   * @brief reference to the singleton ShortcutHandler instance
   */
  ShortcutHandler& m_shortcutHandler;

  Navigator& m_navigator;

  /**
   * @brief reference to the singleton BookmarksRepository instance
   */
  BookmarkService* m_bookmarkService;

  KhatmahService* m_khatmahService;
  /**
   * @brief reference to the singleton QuranRepository instance
   */
  const QuranService* m_quranService;
  /**
   * @brief reference to the singleton TranslationRepository instance
   */
  const TranslationService* m_translationService;
  /**
   * @brief reference to the static QList of available reciters
   */
  const QList<Reciter>& m_reciters;
  /**
   * @brief reference to the static QList of available tafasir
   */
  const QList<Tafsir>& m_tafasir;
  /**
   * @brief initalizes different parts used by the app
   */
  void loadComponents();
  /**
   * @brief load icons for different UI elements
   */
  void loadIcons();
  /**
   * @brief set the current Verse from settings
   */
  void loadVerse();
  /**
   * @brief connect ShortcutHandler signals to their corresponding slots
   */
  void setupShortcuts();
  /**
   * @brief connects signals and slots for different UI components and
   * shortcuts
   */
  void setupConnections();
  /**
   * @brief connect VersePlayer signals to their corresponding slots in
   * MainWindow
   */
  void connectPlayer();
  /**
   * @brief connect SystemTray signals to their corresponding slots in
   * MainWindow
   */
  void connectTray();
  /**
   * @brief connect QuranReader specific signals to their corresponding slots
   */
  void connectReader();
  /**
   * @brief connect Controls/Navigation specific signals to their corresponding
   * slots
   */
  void connectControls();
  /**
   * @brief connect SettingsDialog signals to their corresponding slots
   */
  void connectSettings();
  /**
   * @brief connect menubar actions to their corresponding slots
   */
  void connectMenubar();
  /**
   * @brief register the different notifiers to the NotificationPopup widget
   */
  void connectNotifiers();
  /**
   * @brief initialize the surahs QListView in the side dock and select the
   * current verse's surah
   */
  void setupSurahsDock();
  /**
   * @brief set the QPushButton the menubar that toggles the navigation dock and
   * connect to the appropriate menubar action
   */
  void setupMenubarButton();
  /**
   * @brief sync the surahs QListView in the navigation dock to match the
   * currently active Verse in the VersePlayer
   * @return QModelIndex of the currently selected surah
   */
  QModelIndex syncSelectedSurah();
  /**
   * @brief set the index of the page combobox without signalling other slots
   * @param idx - new index to set the combobox to
   */
  void setCmbPageIdx(int idx);
  /**
   * @brief set the index of the verse combobox without signalling other slots
   * @param idx - new index to set the combobox to
   */
  void setCmbVerseIdx(int idx);
  /**
   * @brief set the index of the juz combobox without signalling other slots
   * @param idx - new index to set the combobox to
   */
  void setCmbJuzIdx(int idx);
  /**
   * @brief flag for navigation slots to ignore signals emitted due to internal
   * change of page combobox index
   */
  bool m_internalPageChange = false;
  /**
   * @brief flag for navigation slots to ignore signals emitted due to internal
   * change of surah combobox index
   */
  bool m_internalSurahChange = false;
  /**
   * @brief flag for navigation slots to ignore signals emitted due to internal
   * change of verse combobox index
   */
  bool m_internalVerseChange = false;
  /**
   * @brief flag for navigation slots to ignore signals emitted due to internal
   * change of juz combobox index
   */
  bool m_internalJuzChange = false;

  /**
   * @brief QList for surah names as it appears in the navigation dock QListView
   */
  QStringList m_surahList;
  /**
   * @brief model used with the navigation dock QListView to display complete
   * list of surahs
   */
  QStringListModel m_surahListModel;

  /**
   * @brief pointer to VersionChecker instance
   */
  QPointer<VersionChecker> m_versionChecker;
  /**
   * @brief pointer to the validator for the editable verse combobox to ensure
   * the number entered is within the surah verse range
   */
  QPointer<QIntValidator> m_verseValidator;

  QPointer<QuranReader> m_reader;
  /**
   * @brief m_playerControls
   *
   * MODIFIED
   */
  QPointer<PlayerControls> m_playerControls;
  /**
   * @brief pointer to SystemTray instance
   */
  QPointer<SystemTray> m_systemTray;
  /**
   * @brief pointer to NotificationPopup instance
   */
  QPointer<NotificationPopup> m_popup;
  /**
   * @brief pointer to VersePlayer instance
   */
  QPointer<PlaybackController> m_playbackController;
  /**
   * @brief pointer to ContentDialog instance
   */
  QPointer<ContentDialog> m_contentDlg;
  /**
   * @brief pointer to SearchDialog instance
   */
  QPointer<SearchDialog> m_searchDlg;
  /**
   * @brief pointer to SettingsDialog instance
   */
  QPointer<SettingsDialog> m_settingsDlg;
  /**
   * @brief pointer to BookmarksDialog instance
   */
  QPointer<BookmarksDialog> m_bookmarksDlg;
  /**
   * @brief pointer to KhatmahDialog instance
   */
  QPointer<KhatmahDialog> m_khatmahDlg;
  /**
   * @brief pointer to CopyDialog instance
   */
  QPointer<CopyDialog> m_cpyDlg;
  /**
   * @brief pointer to DownloaderDialog instance
   */
  QPointer<DownloaderDialog> m_downloaderDlg;
  /**
   * @brief pointer to DownloadManager instance
   */
  QPointer<JobManager> m_jobMgr;
  /**
   * @brief pointer to the surah card (betaqa) widget
   */
  QPointer<BetaqaViewer> m_betaqaViewer;
  /**
   * @brief pointer to the votd dialog
   */
  QPointer<VerseDialog> m_verseDlg;
  /**
   * @brief pointer to the FileSelector dialog used for selecting files for
   * import/export
   */
  QPointer<FileSelector> m_selectorDlg;
  /**
   * @brief pointer to dialog used for selecting which user data parts to
   * import/export
   */
  QPointer<ImportExportDialog> m_importExportDlg;

  QPointer<RepeaterPopup> m_repeater;
};
#endif // MAINWINDOW_H
