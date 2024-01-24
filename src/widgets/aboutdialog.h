#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "../globals.h"
#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AboutDialog(QWidget* parent = nullptr);
  ~AboutDialog();

private:
  Ui::AboutDialog* ui;
  QLocale::Language m_lang = Globals::language;
};

#endif // ABOUTDIALOG_H
