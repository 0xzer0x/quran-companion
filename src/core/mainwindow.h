#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QBoxLayout>
#include <QClipboard>
#include <QIntValidator>
#include <QMainWindow>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include "../utils/dbmanager.h"
#include "../utils/pageconstructor.h"
#include "../utils/verseplayer.h"
#include "../widgets/clickablelabel.h"
#include "../widgets/downloaderdialog.h"
#include "../widgets/searchdialog.h"
#include "../widgets/settingsdialog.h"
#include "../widgets/verseframe.h"
#include "quranpagebrowser.h"

typedef DBManager::Verse Verse;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/*!
 * \class MainWindow
 * 
 * \brief The MainWindow class is responsible for the reader interface, audio controls, navigation, and opening other dialogs
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, QSettings *settingsPtr = nullptr);
    void setupConnections();
    void highlightCurrentVerse();
    enum SideContent { tafsir, translation };
    ~MainWindow();

public slots:
    void saveReaderState();
    void restartApp();

private slots:
    void nextPage();
    void prevPage();
    void btnPlayClicked();
    void btnPauseClicked();
    void btnStopClicked();
    void cmbPageChanged(int newPageIdx);
    void cmbSurahChanged(int newSurahIdx);
    void cmbVerseChanged(int newVerseIdx);
    void updateVerseDropDown();
    void gotoPage(int page);
    void gotoSurah(int surahIdx);
    void activeVerseChanged();
    void spaceKeyPressed();
    void actionPrefTriggered();
    void actionDMTriggered();
    void navigateToVerse(Verse v);
    void updateHighlightColor();
    void showExpandedVerseTafsir();
    void openSearchDialog();
    void missingRecitationFileWarn();
    void mediaStateChanged(QMediaPlayer::PlaybackState state);
    void mediaPosChanged(qint64 position);
    void verseClicked();
    void verseAnchorClicked(const QUrl &hrefUrl);
    void redrawQuranPage();
    void updateSideContentType();
    void updateLoadedTafsir();
    void updateLoadedTranslation();
    void updateSideFont();
    void updateQuranFontSize();
    void addSideContent();

private:
    void init();
    void updateSurah();
    void updatePageVerseInfoList();
    bool m_endOfPage = false;
    bool m_internalPageChange = false;
    bool m_internalSurahChange = false;
    bool m_internalVerseChange = false;
    bool m_darkMode = false;
    Ui::MainWindow *ui;
    QuranPageBrowser *m_quranBrowser;
    QIntValidator *verseValidator = nullptr;
    Verse m_currVerse{1, 1, 1};
    SearchDialog *m_srchDlg = nullptr;
    SettingsDialog *m_settingsDlg = nullptr;
    DownloaderDialog *m_downloaderDlg = nullptr;
    DownloadManager *m_downManPtr = nullptr;
    QSettings *m_settingsPtr;
    SideContent m_sideContent;
    DBManager *m_dbManPtr;
    VersePlayer *m_player;
    PageConstructor *m_pageCon;
    QList<Verse> m_vInfoList;
    QList<QFrame *> m_verseFrameList;
    QTextCursor *m_highlighter;
    QBrush m_highlightColor;
    VerseFrame *m_highlightedFrm = nullptr;
    QDir m_assetsDir = QDir::currentPath() + QDir::separator() + "assets";
    QFont m_sideFont;
    QString m_verseHighlightTemplate;
};
#endif // MAINWINDOW_H
