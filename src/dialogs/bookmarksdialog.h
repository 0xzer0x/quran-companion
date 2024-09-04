/**
 * @file bookmarksdialog.h
 * @brief Header file for BookmarksDialog
 */

#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPointer>
#include <QShortcut>
#include <QStandardItem>
#include <QStringListModel>
#include <QVBoxLayout>
#include <navigation/navigator.h>
#include <repository/bookmarksrepository.h>
#include <repository/glyphsrepository.h>
#include <service/bookmarkservice.h>
#include <service/glyphservice.h>
#include <types/verse.h>
#include <utils/configuration.h>

namespace Ui {
class BookmarksDialog;
}

/**
 * @brief BookmarksDialog is the interface for interacting with saved bookmarks.
 * @details Bookmarks allow the user to save a verse for easier & faster
 * access. Bookmarks are represented as entries in a sqlite database.
 */
class BookmarksDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief BookmarksDialog class constructor
   * @details sets up the UI and displays all bookmarks as the default.
   * @param parent - pointer to the parent widget
   * @param dbMgr - pointer to a DBManager instance for accessing database
   * functionality
   */
  explicit BookmarksDialog(QWidget* parent = nullptr);
  ~BookmarksDialog();

  /**
   * @brief loads all available bookmarks and surahs and selects the default
   * `All` tab.
   */
  void loadDefault();
  /**
   * @brief Reloads the displayed surahs and bookmarks before showing the dialog
   * @details In order for any additions to the bookmarks to appear.
   */
  void showWindow();
  /**
   * @brief Loads all bookmarks or bookmarks from a specific surah.
   * @details If surah parameter is -1 all bookmarks will be loaded, otherwise
   * bookmarks with the surah number passed are loaded. Loaded bookmarks are
   * limited to 10 per page for faster loading of results.
   * @param surah - surah number to load bookmarks from
   */
  void loadBookmarks(int surah = -1);
  /**
   * @brief Loads the surahs of bookmarked verses.
   * @details Surahs are loaded from bookmarks and added to
   * BookmarksDialog::m_surahsModel in order for them to appear in the QListView
   * connected to the model.
   */
  void loadSurahs();

public slots:
  /**
   * @brief generates Verse object from the name of the Frame containing the
   * navigation buttons. Then emits BookmarksDialog::navigateToVerse(Verse).
   */
  void btnGoToVerse();
  /**
   * @brief generates Verse object from the name of the Frame containing the
   * navigation buttons then deletes the corresponding Verse from the
   * bookmarks database.
   * @details After removing the bookmark from the database, the corresponding
   * frame is removed. If there are no more bookmarks in this surah, the default
   * `All` tab is loaded.
   */
  void btnRemove();
  /**
   * @brief Loads the next 10 bookmarks in the currently shown tab.
   */
  void btnNextClicked();
  /**
   * @brief Loads the previous 10 bookmarks in the currently shown tab.
   */
  void btnPrevClicked();

protected:
  /**
   * @brief Re-implementation of QWidget::closeEvent() in order to hide the
   * window instead of closing it.
   * @param event
   */
  void closeEvent(QCloseEvent* event);

private slots:
  /**
   * @brief Slot that loads bookmarks for the clicked surah.
   * @param index - QModelIndex instance for the clicked surah.
   */
  void surahSelected(const QModelIndex& index);

private:
  Ui::BookmarksDialog* ui;
  /**
   * @brief reference to the singleton Configuration instance
   */
  const Configuration& m_config;
  /**
   * @brief reference to the singleton Navigator instance
   */
  Navigator& m_navigator;
  /**
   * @brief reference to the singleton GlyphsRepository instance
   */
  const GlyphService* m_glyphService;
  /**
   * @brief reference to the singleton QuranRepository instance
   */
  const QuranService* m_quranService;
  /**
   * @brief reference to the singleton BookmarksRepository instance
   */
  BookmarkService* m_bookmarkService;
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief adds the 'No bookmarks available' label when bookmarks database is
   * empty.
   */
  void addEmptyBookmarksLabel();
  /**
   * @brief Index of the first bookmark to show in
   * BookmarksDialog::m_shownVerses.
   */
  int m_startIdx = 0;
  /**
   * @brief number of the surah which bookmarks are currently shown (-1 for all
   * bookmarks).
   */
  int m_shownSurah = 0;
  /**
   * @brief Verse QList for all bookmarked verses.
   */
  QList<Verse> m_allBookmarked;
  /**
   * @brief Verse QList for the shown verses (all or for a specific surah).
   */
  QList<Verse> m_shownVerses;
  /**
   * @brief QFrames for shown verses.
   */
  QList<QPointer<QFrame>> m_frames;
  /**
   * @brief model for surahs of bookmarked verses.
   */
  QStandardItemModel m_surahsModel;
};

#endif // BOOKMARKSDIALOG_H
