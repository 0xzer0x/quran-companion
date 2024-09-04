/**
 * @file contentdialog.h
 * @brief Header file for ContentDialog
 */

#ifndef CONTENTDIALOG_H
#define CONTENTDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QShortcut>
#include <repository/bookmarksrepository.h>
#include <repository/glyphsrepository.h>
#include <repository/tafsirrepository.h>
#include <repository/translationrepository.h>
#include <service/bookmarkservice.h>
#include <service/glyphservice.h>
#include <service/tafsirservice.h>
#include <service/thoughtsservice.h>
#include <service/translationservice.h>
#include <types/tafsir.h>
#include <types/translation.h>
#include <types/verse.h>

namespace Ui {
class ContentDialog;
}

/**
 * @brief ContentDialog is used to display any additional content (tafsir,
 * translation, thoughts).
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

public slots:
  /**
   * @brief open ContentDialog with the shown verse set to the given Verse in
   * Mode::Tafsir
   * @param v - Verse to show the tafsir of
   */
  void showVerseTafsir(const Verse& v);
  /**
   * @brief open ContentDialog with the shown verse set to the given Verse in
   * Mode::Translation
   * @param v - Verse to show the translation of
   */
  void showVerseTranslation(const Verse& v);
  /**
   * @brief open ContentDialog with the shown verse set to the given Verse in
   * Mode::Thoughts
   * @param v - Verse to show the thoughts of
   */
  void showVerseThoughts(const Verse& v);

protected:
  /** @brief Re-implementation of QWidget::closeEvent() in order to hide the
   * window instead of closing it.
   * @param event
   */
  void closeEvent(QCloseEvent* event);

signals:
  void missingTafsir(QString id);
  void missingTranslation(QString id);

private slots:
  /**
   * @brief callback for calling the appropriate method when the selected item
   * in the secondary combobox changes
   */
  void contentChanged();
  /**
   * @brief callback for changing the ContentDialog::Mode when the selected mode
   * in the primary combobox changes
   */
  void typeChanged();
  /**
   * @brief increment the m_shownVerse and load the new verse
   * tafsir.
   */
  void btnNextClicked();
  /**
   * @brief decrement the m_shownVerse and load the new verse
   * tafsir.
   */
  void btnPrevClicked();

private:
  Ui::ContentDialog* ui;
  /**
   * @brief reference to the singleton Configuration instance
   */
  Configuration& m_config;
  /**
   * @brief pointer to implementation of TafsirService
   */
  TafsirService* m_tafsirService;
  /**
   * @brief pointer to implementation of TranslationService
   */
  TranslationService* m_translationService;
  /**
   * @brief pointer to implementation of ThoughtsService
   */
  ThoughtsService* m_thoughtsService;
  /**
   * @brief pointer to implementation of BookmarkService
   */
  const BookmarkService* m_bookmarkService;
  /**
   * @brief pointer to implementation of QuranService
   */
  const QuranService* m_quranService;
  /**
   * @brief pointer to implementation of GlyphService
   */
  const GlyphService* m_glyphService;
  /**
   * @brief reference to the static QList of available tafasir
   */
  const QList<::Tafsir>& m_tafasir;
  /**
   * @brief reference to the static QList of available translations
   */
  const QList<::Translation>& m_translations;
  /**
   * @brief loads the QFont selected for the side content
   */
  void loadSideFont();
  /**
   * @brief connects signals and slots for different UI
   * components and shortcuts.
   */
  void setupConnections();
  /**
   * @brief setter member for m_shownVerse
   * @param newShownVerse
   */
  void setShownVerse(const Verse& newShownVerse);
  /**
   * @brief loads the content of Mode given for the currently set Verse
   * @param mode - Mode of the content shown
   */
  void loadContent(Mode mode);
  /**
   * @brief updates the second combobox according to the given Mode
   */
  void updateContentComboBox(Mode mode);
  /**
   * @brief load all available tafasir in the secondary combobox
   */
  void cmbLoadTafasir();
  /**
   * @brief load all available translations in the secondary combobox
   */
  void cmbLoadTranslations();
  /**
   * @brief callback function for changing the selected tafsir in the secondary
   * combobox
   */
  void tafsirChanged();
  /**
   * @brief callback function for changing the selected translation in the
   * secondary combobox
   */
  void translationChanged();
  /**
   * @brief loads the tafsir for the currently shown verse in the QTextEdit
   * widget
   */
  void loadVerseTafsir();
  /**
   * @brief loads the selected translation for the currently shown verse in the
   * QTextEdit widget
   */
  void loadVerseTranslation();
  /**
   * @brief loads the user stored thoughts for the currently shown verse in the
   * QTextEdit widget and enables editing
   */
  void loadVerseThoughts();
  /**
   * @brief saves the user stored thoughts for the currently shown verse in the
   * QTextEdit widget and disables editing
   */
  void saveVerseThoughts();
  /**
   * @brief the current Mode of the ContentDialog
   */
  Mode m_currMode;
  /**
   * @brief index of the currently shown Tafsir in Tafsir::tafasir
   */
  QString m_tafsir;
  /**
   * @brief index of the currently shown Translation in
   * Translation::translations
   */
  QString m_translation;
  /**
   * @brief fixed font size for the verse text displayed above the tafsir.
   */
  int m_fontSZ;
  /**
   * @brief Verse instance representing the shown verse.
   */
  Verse m_shownVerse;
  /**
   * @brief boolean to indicate internal loading of content in the secondary
   * combobox
   */
  bool m_internalLoading;
};

#endif // CONTENTDIALOG_H
