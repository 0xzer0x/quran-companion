/**
 * @file contentdialog.h
 * @brief Header file for ContentDialog
 */

#ifndef CONTENTDIALOG_H
#define CONTENTDIALOG_H

#include "../types/verse.h"
#include "../utils/dbmanager.h"
#include <QDialog>
#include <QSettings>
#include <QShortcut>

namespace Ui {
class ContentDialog;
}

/**
 * @brief ContentDialog interface for reading Quran tafsir.
 * @details Tafsir is shown for a single verse at a time. Navigation between
 * verses is independant of the main Quran reader navigation for easier
 * navigation. Tafsir is displayed using the side content font set in the
 * Settings::settings.
 */
class ContentDialog : public QDialog
{
  Q_OBJECT

public:
  enum Mode
  {
    Tafsir,
    Translation,
    Thoughts
  };
  /**
   * @brief Class constructor
   * @param parent - pointer to parent widget
   */
  explicit ContentDialog(QWidget* parent = nullptr);
  ~ContentDialog();
  /**
   * @brief setter member for ContentDialog::m_shownVerse
   * @param newShownVerse
   */
  void setShownVerse(const Verse& newShownVerse);
  /**
   * @brief loadContent
   * @param mode
   *
   * MODIFIED
   */
  void loadContent(Mode mode);

protected:
  /** @brief Re-implementation of QWidget::closeEvent() in order to hide the
   * window instead of closing it.
   * @param event
   */
  void closeEvent(QCloseEvent* event);

private slots:
  /**
   * @brief contentChanged
   *
   * MODIFIED
   */
  void contentChanged();
  /**
   * @brief typeChanged
   *
   * MODIFIED
   */
  void typeChanged();
  /**
   * @brief increment the ContentDialog::m_shownVerse and load the new verse
   * tafsir.
   */
  void btnNextClicked();
  /**
   * @brief decrement the ContentDialog::m_shownVerse and load the new verse
   * tafsir.
   */
  void btnPrevClicked();

private:
  Ui::ContentDialog* ui;
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  const int m_qcfVer = Settings::qcfVersion;
  const QSharedPointer<QSettings> m_settings = Settings::settings;
  const QList<QSharedPointer<::Tafsir>>& m_tafasir = Tafsir::tafasir;
  const QList<QSharedPointer<::Translation>>& m_translations =
    Translation::translations;
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief setSideFont
   *
   * MODIFIED
   */
  void setSideFont();
  /**
   * @brief updateContentComboBox
   *
   * MODIFIED
   */
  void updateContentComboBox(Mode mode);
  /**
   * @brief cmbLoadTafasir
   *
   * MODIFIED
   */
  void cmbLoadTafasir();
  /**
   * @brief cmbLoadTranslations
   *
   * MODIFIED
   */
  void cmbLoadTranslations();
  /**
   * @brief tafsirChanged
   *
   * MODIFIED
   */
  void tafsirChanged();
  /**
   * @brief translationChanged
   *
   * MODIFIED
   */
  void translationChanged();
  /**
   * @brief loads the info and tafsir of ContentDialog::m_shownVerse
   *
   * MODIFIED
   */
  void loadVerseTafsir();
  /**
   * @brief loadVerseTranslation
   *
   * MODIFIED
   */
  void loadVerseTranslation();
  /**
   * @brief loadVerseThoughts
   *
   * MODIFIED
   */
  void loadVerseThoughts();
  /**
   * MODIFIED
   */
  void saveVerseThoughts();
  /**
   * @brief m_currMode
   *
   * MODIFIED
   */
  Mode m_currMode = Mode::Tafsir;
  int m_tafsir;
  int m_translation;
  /**
   * @brief m_internalLoading
   */
  bool m_internalLoading = false;
  /**
   * @brief fixed font size for the verse text displayed above the tafsir.
   */
  int m_fontSZ;
  /**
   * @brief ::Verse instance representing the shown verse.
   */
  Verse m_shownVerse;
};

#endif // CONTENTDIALOG_H
