/**
 * @file bookmarksdialog.h
 * @brief Header file for BookmarksDialog
 */

#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../utils/verse.h"
#include <QDialog>
#include <QLabel>
#include <QShortcut>
#include <QStandardItem>
#include <QStringListModel>
#include <QVBoxLayout>

namespace Ui {
class BookmarksDialog;
}

/**
 * @brief BookmarksDialog is the interface for interacting with saved bookmarks.
 * @details Bookmarks allow the user to save a verse for easier & faster
 * access. Bookmarks are represented as entries in a sqlite database. Thus, a
 * DBManager instance is required to create an instance of BookmarksDialog.
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

signals:
  /**
   * @fn void navigateToVerse(Verse v)
   * @brief Emitted when 'go to verse' button is clicked to signal the
   * navigation and selection of a verse.
   * @param v - ::Verse to navigate to
   */
  void navigateToVerse(Verse v);

public slots:
  /**
   * @brief generates ::Verse object from the name of the Frame containing the
   * navigation buttons. Then emits BookmarksDialog::navigateToVerse(Verse).
   */
  void btnGoToVerse();
  /**
   * @brief generates ::Verse object from the name of the Frame containing the
   * navigation buttons then deletes the corresponding ::Verse from the
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
  const int m_qcfVer = Globals::qcfVersion;
  DBManager* m_dbMgr = DBManager::instance();
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
   * @brief pointer to access ui elements generated from .ui files.
   */
  Ui::BookmarksDialog* ui;
  /**
   * @brief ::Verse QList for all bookmarked verses.
   */
  QList<Verse> m_allBookmarked;
  /**
   * @brief ::Verse QList for the shown verses (all or for a specific surah).
   */
  QList<Verse> m_shownVerses;
  /**
   * @brief QFrames for shown verses.
   */
  QList<QFrame*> m_frames;
  /**
   * @brief model for surahs of bookmarked verses.
   */
  QStandardItemModel m_surahsModel;
};

#endif // BOOKMARKSDIALOG_H
