/**
 * @file tafsirdialog.h
 * @brief Header file for TafsirDialog
 */

#ifndef TAFSIRDIALOG_H
#define TAFSIRDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include <QDialog>
#include <QSettings>
#include <QShortcut>

namespace Ui {
class TafsirDialog;
}

/**
 * @brief TafsirDialog interface for reading Quran tafsir.
 * @details Tafsir is shown for a single verse at a time. Navigation between
 * verses is independant of the main Quran reader navigation for easier
 * navigation. Tafsir is displayed using the side content font set in the
 * Globals::settings.
 */
class TafsirDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   */
  explicit TafsirDialog(QWidget* parent = nullptr);
  ~TafsirDialog();

  /**
   * @brief loads the info and tafsir of TafsirDialog::m_shownVerse
   */
  void loadVerseTafsir();
  /**
   * @brief setter member for TafsirDialog::m_shownVerse
   * @param newShownVerse
   */
  void setShownVerse(const Verse& newShownVerse);

protected:
  /** @brief Re-implementation of QWidget::closeEvent() in order to hide the
   * window instead of closing it.
   * @param event
   */
  void closeEvent(QCloseEvent* event);
  /**
   * @brief Re-implementation of QWidget::showEvent() in order to change the
   * dialog title to match the current tafsir.
   * @param event
   */
  void showEvent(QShowEvent* event);

private slots:
  /**
   * @brief increment the TafsirDialog::m_shownVerse and load the new verse
   * tafsir.
   */
  void btnNextClicked();
  /**
   * @brief decrement the TafsirDialog::m_shownVerse and load the new verse
   * tafsir.
   */
  void btnPrevClicked();

private:
  const int m_qcfVer = Globals::qcfVersion;
  const QString& m_fontPrefix = Globals::qcfFontPrefix;
  const QSettings* m_settings = Globals::settings;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief sets the Tafsir dialog title to match the displayed tafsir name.
   */
  void setTafsirAsTitle();
  /**
   * @brief Pointer to access ui elements generated from .ui files.
   */
  Ui::TafsirDialog* ui;
  /**
   * @brief fixed font size for the verse text displayed above the tafsir.
   */
  int m_fontSZ;
  /**
   * @brief ::Verse instance representing the shown verse.
   */
  Verse m_shownVerse{ 1, 1, 1 };
};

#endif // TAFSIRDIALOG_H
