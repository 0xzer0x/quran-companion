#include "khatmahdialog.h"
#include "ui_khatmahdialog.h"

KhatmahDialog::KhatmahDialog(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::KhatmahDialog)
{
  ui->setupUi(this);
}

KhatmahDialog::~KhatmahDialog()
{
  delete ui;
}
