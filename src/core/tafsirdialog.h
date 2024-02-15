/**
 * @file tafsirdialog.h
 * @brief Header file for TafsirDialog
 */

#ifndef TAFSIRDIALOG_H
#define TAFSIRDIALOG_H

#include "../types/verse.h"
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
 * Settings::settings.
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
  Ui::TafsirDialog* ui;
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  const int m_qcfVer = Settings::qcfVersion;
  const QSharedPointer<QSettings> m_settings = Settings::settings;
  const QList<QSharedPointer<Tafsir>>& m_tafasirList = Tafsir::tafasir;
  const QList<QSharedPointer<Translation>>& m_trList =
    Translation::translations;
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  void updateContentComboBox();
  /**
   * @brief Tafsir enum value mapped to the tafsir index in the
   * combobox.
   *
   * MODIFIED
   */
  int m_tafsir;
  /**
   * @brief fixed font size for the verse text displayed above the tafsir.
   */
  int m_fontSZ;
  /**
   * @brief ::Verse instance representing the shown verse.
   */
  Verse m_shownVerse;
};

#endif // TAFSIRDIALOG_H
