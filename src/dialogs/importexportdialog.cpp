#include "importexportdialog.h"
#include "ui_importexportdialog.h"

#include <QMessageBox>

ImportExportDialog::ImportExportDialog(QWidget* parent,
                                       QPointer<UserDataImporter> importer,
                                       QPointer<UserDataExporter> exporter)
  : ui(new Ui::ImportExportDialog)
  , m_importer(importer)
  , m_exporter(exporter)
  , QDialog(parent)
{
  ui->setupUi(this);
  connect(ui->buttonBox,
          &QDialogButtonBox::clicked,
          this,
          &ImportExportDialog::dialogButtonClicked);
  connect(m_importer,
          &UserDataImporter::error,
          this,
          &ImportExportDialog::importError);
  connect(m_exporter,
          &UserDataExporter::error,
          this,
          &ImportExportDialog::exportError);
}

void
ImportExportDialog::selectImports(QString filepath)
{
  m_mode = Mode::Import;
  m_importer->setFile(filepath);
  if (m_importer->read())
    setCheckedImports();
  open();
}

void
ImportExportDialog::selectExports(QString filepath)
{
  m_mode = Mode::Export;
  m_exporter->setFile(filepath);
  setCheckedExports();
  open();
}

void
ImportExportDialog::setCheckedImports()
{
  ui->chkBookmarks->setEnabled(m_importer->fileContains("bookmarks"));
  ui->chkKhatmah->setEnabled(m_importer->fileContains("khatmah"));
  ui->chkThoughts->setEnabled(m_importer->fileContains("thoughts"));

  ui->chkBookmarks->setChecked(ui->chkBookmarks->isEnabled());
  ui->chkKhatmah->setChecked(ui->chkKhatmah->isEnabled());
  ui->chkThoughts->setChecked(ui->chkThoughts->isEnabled());
}

void
ImportExportDialog::setCheckedExports()
{
  ui->chkBookmarks->setEnabled(true);
  ui->chkKhatmah->setEnabled(true);
  ui->chkThoughts->setEnabled(true);

  ui->chkBookmarks->setChecked(true);
  ui->chkKhatmah->setChecked(true);
  ui->chkThoughts->setChecked(true);
}

void
ImportExportDialog::importSelected() const
{
  if (ui->chkBookmarks->isChecked())
    m_importer->importBookmarks();
  if (ui->chkKhatmah->isChecked())
    m_importer->importKhatmah();
  if (ui->chkThoughts->isChecked())
    m_importer->importThoughts();
}

void
ImportExportDialog::exportSelected() const
{
  if (ui->chkBookmarks->isChecked())
    m_exporter->exportBookmarks();
  if (ui->chkKhatmah->isChecked())
    m_exporter->exportKhatmah();
  if (ui->chkThoughts->isChecked())
    m_exporter->exportThoughts();

  m_exporter->save();
}

void ImportExportDialog::setExporter(QPointer<UserDataExporter> newExporter)
{
    m_exporter = newExporter;
}

void ImportExportDialog::setImporter(QPointer<UserDataImporter> newImporter)
{
    m_importer = newImporter;
}

void
ImportExportDialog::accept()
{
  switch (m_mode) {
    case Mode::Import:
      importSelected();
      break;
    case Mode::Export:
      exportSelected();
      break;
    default:
      break;
  }
  hide();
}

void
ImportExportDialog::importError(UserDataImporter::Error err, QString str)
{
  QString msg(tr("The following error occured during import") + ":" + "\n%0");
  QMessageBox::warning(this, tr("Error"), msg.arg(str));
  reject();
}

void
ImportExportDialog::exportError(UserDataExporter::Error err, QString str)
{
  QString msg(tr("The following error occured during export") + ":" + "\n%0");
  QMessageBox::warning(this, tr("Error"), msg.arg(str));
  reject();
}

void
ImportExportDialog::dialogButtonClicked(QAbstractButton* btn)
{
  if (ui->buttonBox->buttonRole(btn) == QDialogButtonBox::AcceptRole)
    accept();
  else
    reject();
}

ImportExportDialog::~ImportExportDialog()
{
  delete ui;
}
