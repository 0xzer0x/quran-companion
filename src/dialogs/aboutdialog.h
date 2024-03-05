#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <QLocale>
#include <utils/configuration.h>

namespace Ui {
class AboutDialog;
}

/**
 * @class AboutDialog
 * @brief AboutDialog class is used for displaying information about the
 * application
 */
class AboutDialog : public QDialog
{
  Q_OBJECT

public:
  /**
   * @brief class constructor
   * @param parent - pointer to parent widget
   */
  explicit AboutDialog(QWidget* parent = nullptr);
  ~AboutDialog();

private:
  Ui::AboutDialog* ui;
  /**
   * @brief reference to the singleton Configuration instance
   */
  const Configuration& m_config;
};

#endif // ABOUTDIALOG_H
