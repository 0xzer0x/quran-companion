/**
 * @file searchdialog.h
 * @brief Header file for SearchDialog
 */

#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QPointer>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>
#include <navigation/navigator.h>
#include <repository/glyphsrepository.h>
#include <service/glyphservice.h>
#include <service/quranservice.h>
#include <types/verse.h>
#include <widgets/verseframe.h>

namespace Ui {
class SearchDialog;
}

/**
 * @brief SearchDialog is an interface for searching Quran verses.
 * @details Searching Quran verses is done through SQL Queries to the Quran
 * sqlite database. Searching options include using whole-word search, searching
 * within a page range, and searching within specific surahs only.
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
   * selected surahs. Results are displayed and navigation buttons are updated
   * based on the search results.
   */
  void getResults();
  /**
   * @brief Slot that is called when one of the result verse labels is clicked.
   * Extracts a Verse from the emitting object name and emits
   * SearchDialog::navigateToVerse(Verse) signal.
   */
  void verseClicked();
  /**
   * @brief Displays 25 entries from the search results.
   * @details Search results are limited to 25 results per page. The starting
   * index for the results to display is held in SearchDialog::m_startResult.
   */
  void showResults();
  /**
   * @brief Loads the next 25 search results in the results tab.
   * @details Updates the view to show the next set of 25 results if available,
   * and adjusts the navigation buttons accordingly.
   */
  void moveFwd();
  /**
   * @brief Loads the previous 25 search results in the results tab.
   * @details Updates the view to show the previous set of 25 results if
   * available, and adjusts the navigation buttons accordingly.
   */
  void moveBwd();

protected:
  /**
   * @brief Re-implementation of QWidget::closeEvent() to hide the window
   * instead of closing it.
   * @param event - The close event
   */
  void closeEvent(QCloseEvent* event) override;

private slots:
  /**
   * @brief Moves selected surahs between the surah filtering QListViews.
   * @details Surah-specific search is done by passing a QList of integers
   * representing the selected surah numbers to the database manager. Selected
   * surahs are stored as key-value pairs in SearchDialog::m_selectedSurahMap,
   * where the key is the surah name as it appears on-screen, and the value is
   * the surah number.
   */
  void btnTransferClicked();

private:
  /**
   * @brief Pointer to the UI object for the dialog.
   */
  Ui::SearchDialog* ui;
  /**
   * @brief Reference to the Navigator instance used for navigating to verses.
   */
  Navigator& m_navigator;
  /**
   * @brief Reference to the Configuration instance holding application
   * settings.
   */
  const Configuration& m_config;
  /**
   * @brief Pointer to the QuranService instance for accessing Quran data.
   */
  const QuranService* m_quranService;
  /**
   * @brief Pointer to the GlyphService instance for accessing verse glyphs.
   */
  const GlyphService* m_glyphService;
  /**
   * @brief Connects signals and slots for different UI components and
   * shortcuts.
   */
  void setupConnections();
  /**
   * @brief Populates the QListView with surah names for selection.
   */
  void fillListView();
  /**
   * @brief Index of the first search result to show in
   * SearchDialog::m_currResults.
   */
  int m_startResult = 0;
  /**
   * @brief List for all visible VerseFrame widgets containing search results.
   */
  QList<QPointer<VerseFrame>> m_lbLst;
  /**
   * @brief List of verses for the current search results.
   */
  QList<Verse> m_currResults;
  /**
   * @brief Map of surah names to surah numbers for surahs selected for
   * searching.
   */
  QMap<QString, int> m_selectedSurahMap;
  /**
   * @brief Current search text entered by the user.
   */
  QString m_searchText;
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
