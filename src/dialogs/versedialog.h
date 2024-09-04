#ifndef VERSEDIALOG_H
#define VERSEDIALOG_H

#include <QDateTime>
#include <QDialog>
#include <navigation/navigator.h>
#include <repository/translationrepository.h>
#include <service/quranservice.h>
#include <service/translationservice.h>
#include <types/verse.h>
#include <utils/dirmanager.h>

namespace Ui {
class VerseDialog;
}

/**
 * @class VerseDialog
 * @brief A dialog that displays the Verse of the Day (VOTD) from the Quran.
 *
 * This class provides a user interface for showing the verse of the day,
 * including the verse text, its translation, and additional information. It
 * manages the generation, reading, and writing of the VOTD to a log file to
 * ensure that the verse is updated daily.
 */
class VerseDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a VerseDialog object.
   * @param parent The parent widget for this dialog.
   */
  explicit VerseDialog(QWidget* parent = nullptr);

  /**
   * @brief Destroys the VerseDialog object.
   */
  ~VerseDialog();

  /**
   * @brief Checks if the Verse of the Day (VOTD) has been displayed for the
   * current day.
   * @return True if the VOTD has been shown, false otherwise.
   */
  bool votdShown();

public slots:
  /**
   * @brief Displays the Verse of the Day (VOTD).
   *
   * If the VOTD has not been shown today, this method will generate a new VOTD,
   * write the timestamp to the log file, and update the labels. The startup
   * parameter determines whether the dialog should be shown on startup.
   * @param startup Whether the dialog is being shown at startup.
   */
  void showVOTD(bool startup);

protected:
  /**
   * @brief Handles mouse release events.
   *
   * Navigates to the verse associated with the Verse of the Day and hides the
   * dialog.
   * @param event The mouse event that triggered this function.
   */
  void mouseReleaseEvent(QMouseEvent* event) override;

  /**
   * @brief Handles close events.
   *
   * Hides the dialog when it is closed.
   * @param event The close event that triggered this function.
   */
  void closeEvent(QCloseEvent* event) override;

private:
  Ui::VerseDialog*
    ui; ///< Pointer to the user interface components of the dialog. //
  Configuration&
    m_config; ///< Reference to the application configuration instance. //
  Navigator& m_navigator; ///< Reference to the navigator instance for handling
                          ///< verse navigation. //
  const QuranService* m_quranService; ///< Pointer to the Quran service for
                                      ///< fetching verse texts. //
  const TranslationService*
    m_translationService; ///< Pointer to the translation service for fetching
                          ///< translations. //
  QFile m_timestampFile;  ///< File for logging the timestamp and details of the
                          ///< Verse of the Day. //
  /**
   * @brief Generates a new Verse of the Day (VOTD) and updates the VOTD HTML.
   */
  void genVerseOfTheDay();

  /**
   * @brief Reads the Verse of the Day (VOTD) from the timestamp file and
   * updates the dialog.
   */
  void readVerseOfTheDay();

  /**
   * @brief Writes the current timestamp and the Verse of the Day (VOTD) to the
   * timestamp file.
   */
  void writeTimestamp();

  /**
   * @brief Serializes the Verse of the Day (VOTD) into a QString in the format
   * PAGE:SURA:NUMBER.
   * @return QString containing the page, surah, and verse number of the VOTD.
   */
  QString votdStringEntry() const;

  /**
   * @brief Updates the labels in the dialog with the current Verse of the Day
   * (VOTD) information.
   */
  void updateLabels();

  Verse m_votd; ///< The current Verse of the Day (VOTD). */
  QDateTime m_now =
    QDateTime::currentDateTime(); ///< The current date and time. */
};

#endif // VERSEDIALOG_H
