#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../utils/notificationmanager.h"
#include "../utils/verseplayer.h"
#include "../widgets/notificationpopup.h"
#include "../widgets/quranpagebrowser.h"
#include "../widgets/verseframe.h"
#include "bookmarksdialog.h"
#include "downloaderdialog.h"
#include "searchdialog.h"
#include "settingsdialog.h"
#include "tafsirdialog.h"
#include <QBoxLayout>
#include <QClipboard>
#include <QDesktopServices>
#include <QIntValidator>
#include <QMainWindow>
#include <QPropertyAnimation>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \class MainWindow
 *
 * \brief The MainWindow class is responsible for the reader interface, audio
 * controls, navigation, and opening other dialogs
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  void highlightCurrentVerse();

public slots:
  void showVOTDmessage(QPair<Verse, QString> votd);
  void checkForUpdates();
  void updateProcessCallback();
  void saveReaderState();
  void restartApp();

  // QWidget interface
protected:
  void resizeEvent(QResizeEvent* event);

private slots:
  void nextPage();
  void prevPage();
  void gotoPage(int page, bool automaticFlip = false);
  void gotoSurah(int surahIdx);
  void setVerseComboBoxRange(bool forceUpdate = false);

  void btnPlayClicked();
  void btnPauseClicked();
  void btnStopClicked();

  void cmbPageChanged(int newPageIdx);
  void cmbVerseChanged(int newVerseIdx);
  void cmbJozzChanged(int newJozzIdx);

  void activeVerseChanged();
  void missingRecitationFileWarn(int reciterIdx, int surah);
  void mediaPosChanged(qint64 position);
  void mediaStateChanged(QMediaPlayer::PlaybackState state);

  void addCurrentToBookmarks();
  void spaceKeyPressed();
  void actionPrefTriggered();
  void actionDMTriggered();
  void actionBookmarksTriggered();
  void actionTafsirTriggered();
  void actionVotdTriggered();
  void openSearchDialog();
  void actionAboutTriggered();
  void on_actionAbout_Qt_triggered();

  void verseClicked();
  void surahClicked(QModelIndex& index);
  void showExpandedVerseTafsir(Verse v);
  void navigateToVerse(Verse v);
  void verseAnchorClicked(const QUrl& hrefUrl);
  void copyVerseText(int IdxInPage);

  void redrawQuranPage(bool manualSz = false);
  void addSideContent();
  void updateTrayTooltip(QMediaPlayer::PlaybackState newState);

  void updateLoadedTafsir();
  void updateLoadedTranslation();
  void updateSideFont();

  void searchSurahTextChanged(const QString& arg1);
  void listSurahNameClicked(const QModelIndex& index);
  void volumeSliderValueChanged(int position);

private:
  QSettings* const m_settings = Globals::settings;
  const QList<Reciter>& m_recitersList = Globals::recitersList;
  const QDir m_resources = Globals::themeResources;
  const QString& m_updateToolPath = Globals::updateToolPath;
  const bool m_darkMode = Globals::darkMode;
  void init();
  void loadIcons();
  void loadSettings();
  void setupConnections();
  void setupSurahsDock();
  void setupMenubarToggle();
  void updateSurah();
  void updatePageVerseInfoList();
  void setVerseToStartOfPage();
  void setCmbPageIdx(int idx);
  void setCmbVerseIdx(int idx);
  void setCmbJozzIdx(int idx);
  bool m_endOfPage = false;
  bool m_internalPageChange = false;
  bool m_internalSurahChange = false;
  bool m_internalVerseChange = false;
  bool m_internalJozzChange = false;
  qreal m_volume = 1;
  Ui::MainWindow* ui;
  QuranPageBrowser* m_quranBrowser;
  NotificationManager* m_notifyMgr;
  NotificationPopup* m_popup = nullptr;
  DBManager* m_dbMgr;
  VersePlayer* m_player;
  TafsirDialog* m_tafsirDlg = nullptr;
  SearchDialog* m_searchDlg = nullptr;
  SettingsDialog* m_settingsDlg = nullptr;
  BookmarksDialog* m_bookmarksDlg = nullptr;
  DownloaderDialog* m_downloaderDlg = nullptr;
  DownloadManager* m_downManPtr = nullptr;
  HighlightFrame* m_highlightedFrm = nullptr;
  Verse m_currVerse{ 1, 1, 1 };
  QList<QFrame*> m_verseFrameList;
  QList<Verse> m_vInfoList;
  QProcess* m_process;
  QIntValidator* m_verseValidator = nullptr;
  QStringList m_surahList;
  QStringListModel m_surahListModel;
  QDir m_assetsDir;
  QFont m_sideFont;
};
#endif // MAINWINDOW_H
