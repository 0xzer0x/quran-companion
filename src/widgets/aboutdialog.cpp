#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::AboutDialog)
{
  ui->setupUi(this);
  ui->aboutTabWidget->setLayoutDirection(Qt::LeftToRight);
  ui->lbVersionNum->setText(qApp->applicationVersion());
  connect(ui->btnBox, &QDialogButtonBox::accepted, this, &QDialog::close);
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
