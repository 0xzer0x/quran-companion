/**
 * @file importexportdialog.h
 * @brief Header file for the ImportExportDialog class, which provides a dialog
 * interface for importing and exporting user data.
 */

#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QPointer>
#include <serializer/userdataexporter.h>
#include <serializer/userdataimporter.h>

namespace Ui {
class ImportExportDialog;
}

/**
 * @brief The ImportExportDialog class provides a dialog for importing and
 * exporting user data.
 * @details This dialog allows users to select files for importing or exporting
 * user data and provides options to choose specific types of data (e.g.,
 * bookmarks, khatmah, thoughts) to be imported or exported. It handles error
 * reporting and user interactions with the dialog buttons.
 */
class ImportExportDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief Enum defining the modes for the dialog.
   */
  enum Mode
  {
    Import, ///< Mode for importing data.
    Export  ///< Mode for exporting data.
  };
  /**
   * @brief Constructor for ImportExportDialog.
   * @param parent Pointer to the parent widget.
   * @param importer Shared pointer to a UserDataImporter instance for importing
   * data.
   * @param exporter Shared pointer to a UserDataExporter instance for exporting
   * data.
   */
  explicit ImportExportDialog(QWidget* parent,
                              QSharedPointer<UserDataImporter> importer,
                              QSharedPointer<UserDataExporter> exporter);
  ~ImportExportDialog();

  /**
   * @brief Configures the dialog for importing data from the specified file.
   * @param filepath Path to the file to import data from.
   */
  void selectImports(QString filepath);
  /**
   * @brief Configures the dialog for exporting data to the specified file.
   * @param filepath Path to the file to export data to.
   */
  void selectExports(QString filepath);
  /**
   * @brief Sets a new UserDataImporter instance.
   * @param newImporter Shared pointer to the new UserDataImporter instance.
   */
  void setImporter(QSharedPointer<UserDataImporter> newImporter);
  /**
   * @brief Sets a new UserDataExporter instance.
   * @param newExporter Shared pointer to the new UserDataExporter instance.
   */
  void setExporter(QSharedPointer<UserDataExporter> newExporter);

private slots:
  /**
   * @brief Handles button clicks in the dialog's button box.
   * @param btn Pointer to the clicked button.
   */
  void dialogButtonClicked(QAbstractButton* btn);
  /**
   * @brief Handles errors during data import.
   * @param err Error code for the import error.
   * @param str Error message for the import error.
   */
  void importError(UserDataImporter::Error err, QString str);
  /**
   * @brief Handles errors during data export.
   * @param err Error code for the export error.
   * @param str Error message for the export error.
   */
  void exportError(UserDataExporter::Error err, QString str);
  /**
   * @brief Accepts the dialog and performs the selected import or export
   * action.
   */
  void accept();

private:
  Ui::ImportExportDialog* ui; ///< Pointer to the dialog's UI.
  QSharedPointer<UserDataImporter>
    m_importer; ///< Shared pointer to the UserDataImporter instance.
  QSharedPointer<UserDataExporter>
    m_exporter; ///< Shared pointer to the UserDataExporter instance.
  Mode m_mode;  ///< Current mode of the dialog (Import or Export).
  /**
   * @brief Updates the state of the import checkboxes based on the import file.
   */
  void setCheckedImports();

  /**
   * @brief Updates the state of the export checkboxes to be checked.
   */
  void setCheckedExports();

  /**
   * @brief Performs the selected import actions based on the checkbox states.
   */
  void importSelected() const;

  /**
   * @brief Performs the selected export actions based on the checkbox states.
   */
  void exportSelected() const;
};

#endif // IMPORTEXPORTDIALOG_H
