#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QClipboard>
#include <QDialog>
#include <QIntValidator>
#include <QMessageBox>
#include <QPointer>
#include <notifiers/copynotifier.h>
#include <service/quranservice.h>
#include <types/verse.h>

namespace Ui {
class CopyDialog;
}
/**
 * @class CopyDialog
 * @brief The CopyDialog class is used for selecting a range of verses within
 * the current surah to copy to clipboard or copying the current Verse
 */
class CopyDialog : public QDialog
{
  Q_OBJECT
public:
  /**
   * @brief class consrtuctor
   * @param parent - pointer to parent widget
   */
  explicit CopyDialog(QWidget* parent);
  ~CopyDialog();

  /**
   * @brief sets the currently active surah name in the corresponding QLabel and
   * fills the range selection comboboxes before showing the CopyDialog
   */
  void show();
  /**
   * @brief getter for the address of the CopyNotifier used for sending
   * notifications
   * @return pointer to CopyNotifier instance
   */
  const CopyNotifier* notifier() const;

public slots:
  /**
   * @brief copy to clipboard the text of the verse with the given index
   * @param IdxInPage - verse index relative to the start of the page
   */
  void copyVerseText(const Verse v);

protected:
  void closeEvent(QCloseEvent* event);

private slots:
  void copyRange();

private:
  Ui::CopyDialog* ui;
  /**
   * @brief reference to the singleton QuranRepository instance
   */
  const QuranService* m_quranService;
  /**
   * @brief reference to the shared current verse instance
   */
  const Verse& m_currVerse;
  /**
   * @brief pointer to QIntValidator used for validating the selected range of
   * verses
   */
  QPointer<QIntValidator> m_verseValidator;
  /**
   * @brief CopyNotifier instance used for sending notifications
   */
  CopyNotifier m_notifier;
};

#endif // COPYDIALOG_H
