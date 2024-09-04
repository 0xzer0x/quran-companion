#ifndef KHATMAHDIALOG_H
#define KHATMAHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPointer>
#include <QSet>
#include <navigation/navigator.h>
#include <repository/bookmarksrepository.h>
#include <service/bookmarkservice.h>
#include <service/khatmahservice.h>
#include <service/quranservice.h>
#include <types/verse.h>
#include <widgets/inputfield.h>

namespace Ui {
class KhatmahDialog;
}

/**
 * @brief KhatmahDialog is responsible for managing and tracking different
 * Khatmah entries. This dialog allows users to view, create, rename, and delete
 * Khatmah entries, as well as set an active Khatmah and navigate to the
 * corresponding verse.
 */
class KhatmahDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a KhatmahDialog instance.
   * @param parent - Pointer to the parent widget. Default is nullptr.
   */
  explicit KhatmahDialog(QWidget* parent = nullptr);

  /**
   * @brief Destructs the KhatmahDialog instance.
   */
  ~KhatmahDialog();

  /**
   * @brief Reloads the shown Khatmah entries and displays the dialog.
   */
  void show();

protected:
  /**
   * @brief Re-implementation of QWidget::closeEvent() to hide the dialog
   * instead of closing it.
   * @param event - The close event.
   */
  void closeEvent(QCloseEvent* event) override;

private slots:
  /**
   * @brief Adds a new Khatmah entry with the default name "Khatmah N", where N
   * is the ID of the Khatmah entry.
   */
  void startNewKhatmah();

  /**
   * @brief Renames the Khatmah associated with the sender InputField to the
   * provided name.
   * @param name - The new name for the Khatmah.
   */
  void renameKhatmah(QString name);

  /**
   * @brief Removes the Khatmah associated with the sender QPushButton.
   */
  void removeKhatmah();

  /**
   * @brief Sets the Khatmah associated with the sender QPushButton as the
   * active Khatmah and navigates to it.
   */
  void setActiveKhatmah();

private:
  /**
   * @brief Loads all Khatmah entries and displays them in the dialog.
   */
  void loadAll();

  /**
   * @brief Loads a Khatmah entry with the specified ID.
   * @param id - The ID of the Khatmah to load.
   * @return A pointer to the InputField associated with the loaded Khatmah
   * name.
   */
  QPointer<InputField> loadKhatmah(const int id);

  Ui::KhatmahDialog* ui;        ///< Pointer to the UI elements of the dialog.
  const Verse& m_currVerse;     ///< Reference to the current active verse.
  QuranService* m_quranService; ///< Pointer to the QuranService instance.
  KhatmahService* m_khatmahService; ///< Pointer to the KhatmahService instance.
  Configuration& m_config; ///< Reference to the Configuration instance.
  Navigator& m_navigator;  ///< Reference to the Navigator instance.
  QPointer<QFrame>
    m_currActive; ///< Pointer to the currently active Khatmah QFrame.
  QList<QPointer<QFrame>>
    m_frmLst; ///< List of all QFrame(s) loaded for different Khatmah entries.
  QList<int> m_khatmahIds; ///< List of all available Khatmah IDs.
  QHash<int, QString>
    m_names; ///< Hash map of Khatmah IDs and their associated names.
};

#endif // KHATMAHDIALOG_H
