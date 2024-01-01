#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include <QClipboard>
#include <QDialog>
#include <QIntValidator>
#include <QMessageBox>

namespace Ui {
class CopyDialog;
}

class CopyDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CopyDialog(QWidget* parent);
  void show(const Verse& curr);
  ~CopyDialog();

protected:
  void closeEvent(QCloseEvent* event);

private:
  Ui::CopyDialog* ui;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  void copyRange();
  int m_surah = -1;
  int m_surahCnt = 0;
  QIntValidator* m_verseValidator = new QIntValidator(this);
};

#endif // COPYDIALOG_H
