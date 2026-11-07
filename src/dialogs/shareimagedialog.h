#ifndef SHAREIMAGEDIALOG_H
#define SHAREIMAGEDIALOG_H

#include <QDialog>
#include <QImage>
#include <QTextDocument>
#include <navigation/verseobserver.h>
#include <service/quranservice.h>
#include <service/tafsirservice.h>
#include <service/translationservice.h>
#include <types/verse.h>

namespace Ui {
class ShareImageDialog;
}

/**
 * @class ShareImageDialog
 * @brief The ShareImageDialog class provides a dialog for rendering a range of
 * verses from the current surah, optionally with a tafsir or translation
 * beneath each verse, to an image. The image is previewed in the dialog
 * before being saved.
 */
class ShareImageDialog
  : public QDialog
  , public VerseObserver
{
  Q_OBJECT
public:
  /**
   * @brief Constructor for the ShareImageDialog class.
   * @param parent - Pointer to the parent widget.
   */
  explicit ShareImageDialog(QWidget* parent);
  ~ShareImageDialog();

  /**
   * @brief Shows the dialog for the currently highlighted verse.
   */
  void show();
  /**
   * @brief Re-populates the dialog when the highlighted verse changes while it
   * is visible. The range follows the verse, unless a multi-verse range was
   * picked in the same surah.
   */
  void activeVerseChanged() override;

public slots:
  /**
   * @brief Prepares the dialog for a verse, selecting it as the range and
   * rendering the preview. Then, it shows the dialog.
   * @param verse - the verse to share
   */
  void showVerse(const Verse& verse);

protected:
  /**
   * @brief Re-renders the preview to fit the new dialog width.
   * @param event - The QResizeEvent object representing the resize event.
   */
  void resizeEvent(QResizeEvent* event) override;

private slots:
  /**
   * @brief Lays out the selected range in ::m_doc and updates the preview.
   */
  void updatePreview();
  /**
   * @brief Renders ::m_doc as a PNG to a user-selected path.
   */
  void saveImage();

private:
  /**
   * @brief The kind of content shown beneath each verse.
   */
  enum ContentKind
  {
    NoContent,
    TafsirContent,
    TranslationContent
  };
  /**
   * @brief Item data roles of the content combobox.
   */
  enum ContentRole
  {
    KindRole = Qt::UserRole,
    IdRole
  };
  Ui::ShareImageDialog* ui;
  /**
   * @brief Reference to the shared current verse instance, representing
   * the verse currently being displayed.
   */
  const Verse& m_currVerse;
  /**
   * @brief The verse the dialog was shown for, its surah is the one rendered.
   */
  Verse m_verse;
  /**
   * @brief Pointer to the QuranService instance used for accessing verse
   * data.
   */
  const QuranService* m_quranService;
  /**
   * @brief Pointer to the TafsirService instance used for accessing the
   * tafsir of the verses.
   */
  TafsirService* m_tafsirService;
  /**
   * @brief Pointer to the TranslationService instance used for accessing the
   * translation of the verses.
   */
  TranslationService* m_translationService;
  /**
   * @brief The laid out document of the selected range, empty if the range is
   * invalid. It is rendered at the preview size and at full size on save.
   */
  QTextDocument m_doc;
  /**
   * @brief Fills the content combobox with the available tafasir and
   * translations, keeping the current selection if still available.
   */
  void populateContent();
  /**
   * @brief Gets the selected content of each verse in a range of the current
   * surah, switching the shared tafsir/translation temporarily.
   * @param from - first verse number
   * @param to - last verse number
   * @param name - set to the display name of the selected content
   * @return HTML of the content of each verse, empty if no content is selected
   */
  QStringList contentHtml(int from, int to, QString& name);
  /**
   * @brief Renders ::m_doc to an image.
   * @param scale - scale factor relative to the full image width
   * @return QImage of the rendered document
   */
  QImage renderImage(qreal scale) const;
  /**
   * @brief Renders ::m_doc at the preview label width and device pixel ratio.
   */
  void showPreview();
};

#endif // SHAREIMAGEDIALOG_H
