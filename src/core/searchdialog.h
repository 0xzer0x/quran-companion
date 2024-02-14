/**
 * @file searchdialog.h
 * @brief Header file for SearchDialog
 */

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../utils/verse.h"
#include "../widgets/verseframe.h"
#include <QDialog>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>

namespace Ui {
class SearchDialog;
}

/**
 * @brief SearchDialog is an interface for searching Quran verses.
 * @details Searching Quran verses is done through SQL Queries to the Quran
 * sqlite database. Searching options include using whole-word search, searching
 * withing a page range, and searching within specific surahs only.
 */
class SearchDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   */
  explicit SearchDialog(QWidget* parent = nullptr);
  ~SearchDialog();

public slots:
  /**
   * @brief Slot to get search results and update UI accordingly.
   * @details Search queries are made using either a page range (default) or
   * selected surahs.
   */
  void getResults();
  /**
   * @brief Slot that is called when one of the result verse labels is clicked.
   * Extracts a ::Verse from the emitting object name and emits
   * SearchDialog::navigateToVerse(Verse) signal.
   */
  void verseClicked();
  /**
   * @brief Displays 25 entry from the search results.
   * @details Search results are limited to 25 result per page.
   * SearchDialog::m_startIdx holds the index to start from.
   */
  void showResults();
  /**
   * @brief Loads the next 25 search result in the results tab.
   */
  void moveFwd();
  /**
   * @brief Loads the previous 25 search result in the results tab.
   */
  void moveBwd();

signals:
  /**
   * @fn void navigateToVerse(Verse v)
   * @brief Emitted when a search result is clicked to signal the
   * navigation and selection of that verse.
   * @param v - ::Verse to navigate to
   */
  void navigateToVerse(Verse v);

protected:
  /** @brief Re-implementation of QWidget::closeEvent() in order to hide the
   * window instead of closing it.
   * @param event
   */
  void closeEvent(QCloseEvent* event);

private slots:
  /**
   * @brief move selected surahs between the surah filtering QListViews.
   * @details Surah-specific search is done through passing a QList of integers
   * representing the selected surah numbers to the DBManager member function.
   * Selected surahs are stored as key-value pairs in
   * SearchDialog::m_selectedSurahMap where the key is the surah name as it
   * appears on-screen and the value is the surah number.
   */
  void btnTransferClicked();

private:
  const QLocale::Language m_lang = Globals::language;
  DBManager* m_dbMgr = DBManager::instance();
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief Populates the QListView through which the user selects the surahs to
   * search.
   */
  void fillListView();
  /**
   * @brief Index of the first search result to show in
   * SearchDialog::m_currResults.
   */
  int m_startResult = 0;
  /**
   * @brief Pointer to access ui elements generated from .ui files.
   */
  Ui::SearchDialog* ui;
  /**
   * @brief QList for all visible HighlightFrame widgets containing search
   * results.
   */
  QList<VerseFrame*> m_lbLst;
  /**
   * @brief ::Verse QList for the current search results.
   */
  QList<Verse> m_currResults;
  /**
   * @brief Surah name-number QMap for surahs selected for searching.
   */
  QMap<QString, int> m_selectedSurahMap;
  /**
   * @brief Current search text.
   */
  QString m_searchText;
  /**
   * @brief QStringList for the surah names which are displayed above search
   * results and in the QListView.
   */
  QStringList m_surahNames;
  /**
   * @brief Model for the QListView that shows all surahs to select from.
   */
  QStandardItemModel m_modelAllSurahs;
  /**
   * @brief Model for the QListView that shows selected surahs.
   */
  QStandardItemModel m_modelSelectedSurahs;
};

#endif // SEARCHDIALOG_H
