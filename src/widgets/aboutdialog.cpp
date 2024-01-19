#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->translatorsTab->setLayoutDirection(Qt::LeftToRight);
  ui->lbVersionNum->setText(qApp->applicationVersion());
  ui->lbLicense->setText(ui->lbLicense->text().arg(
    tr("Licensed under the"), tr("Waqf General Public License")));
  ui->lbHomepage->setText(ui->lbHomepage->text().arg(tr("Project Homepage")));
  ui->lbBugFeatureReq->setText(
    ui->lbBugFeatureReq->text().arg(tr("Report a bug/Request a feature")));
  ui->lbContribTranslation->setText(
    ui->lbContribTranslation->text().arg(tr("Contribute to translations")));
  connect(ui->btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  if (m_lang == QLocale::Arabic)
    ui->aboutTabWidget->setObjectName("rtlTabWidget");
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
