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
 * @brief The CopyDialog class provides a dialog interface for selecting and
 * copying a range of verses from the current surah to the clipboard. It also
 * allows copying a single verse with proper formatting.
 */
class CopyDialog : public QDialog
{
  Q_OBJECT
public:
  /**
   * @brief Constructor for the CopyDialog class.
   * @param parent - Pointer to the parent widget.
   */
  explicit CopyDialog(QWidget* parent);
  ~CopyDialog();

  /**
   * @brief Prepares the dialog by setting the currently active surah name
   * in the corresponding QLabel and populating the range selection comboboxes.
   * Then, it shows the dialog.
   */
  void show();
  /**
   * @brief Getter for the address of the CopyNotifier used for sending
   * notifications.
   * @return Pointer to the CopyNotifier instance.
   */
  NotificationSender* notifier();

public slots:
  /**
   * @brief Copies the text of the specified verse to the clipboard, with
   * formatting that includes the verse number and surah name.
   * @param v - The Verse object representing the verse to copy.
   */
  void copyVerseText(const Verse v);

protected:
  /**
   * @brief Handles the close event for the dialog, hiding the dialog
   * instead of closing it.
   * @param event - The QCloseEvent object representing the close event.
   */
  void closeEvent(QCloseEvent* event);

private slots:
  /**
   * @brief Copies a range of verses, specified by the combobox values,
   * to the clipboard. The copied text includes the verse numbers and
   * surah name with proper formatting.
   */
  void copyRange();

private:
  Ui::CopyDialog* ui;
  /**
   * @brief Reference to the QuranService instance used for accessing verse
   * data.
   */
  const QuranService* m_quranService;
  /**
   * @brief Reference to the shared current verse instance, representing
   * the verse currently being displayed or manipulated.
   */
  const Verse& m_currVerse;
  /**
   * @brief Pointer to a QIntValidator used for validating the range
   * of verses entered by the user.
   */
  QPointer<QIntValidator> m_verseValidator;
  /**
   * @brief Instance of CopyNotifier used for sending notifications when
   * verses are copied to the clipboard.
   */
  CopyNotifier m_notifier;
};

#endif // COPYDIALOG_H
