#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->translatorsTab->setLayoutDirection(Qt::LeftToRight);
  ui->lbVersionNum->setText(qApp->applicationVersion());
  connect(ui->btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  if (m_lang == QLocale::Arabic)
    ui->aboutTabWidget->setObjectName("rtlTabWidget");
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
