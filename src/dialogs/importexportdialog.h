#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QPointer>
#include <interfaces/userdataexporter.h>
#include <interfaces/userdataimporter.h>

namespace Ui {
class ImportExportDialog;
}

class ImportExportDialog : public QDialog
{
  Q_OBJECT
public:
  enum Mode
  {
    Import,
    Export
  };
  explicit ImportExportDialog(QWidget* parent,
                              QPointer<UserDataImporter> importer,
                              QPointer<UserDataExporter> exporter);
  ~ImportExportDialog();

  void selectImports(QString filepath);
  void selectExports(QString filepath);

  void setImporter(QPointer<UserDataImporter> newImporter);
  void setExporter(QPointer<UserDataExporter> newExporter);

private slots:
  void dialogButtonClicked(QAbstractButton* btn);
  void importError(UserDataImporter::Error err, QString str);
  void exportError(UserDataExporter::Error err, QString str);
  void accept();

private:
  Ui::ImportExportDialog* ui;
  void setCheckedImports();
  void setCheckedExports();
  void importSelected() const;
  void exportSelected() const;
  QPointer<UserDataImporter> m_importer;
  QPointer<UserDataExporter> m_exporter;
  Mode m_mode;
};

#endif // IMPORTEXPORTDIALOG_H
