#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include "../utils/dbmanager.h"
#include "../types/verse.h"
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
  ~CopyDialog();

  void show();

public slots:
  /**
   * @brief copy to clipboard the text of the verse with the given index
   * @param IdxInPage - verse index relative to the start of the page
   */
  void copyVerseText(const Verse v);

signals:
  void verseCopied();
  void rangeCopied();

protected:
  void closeEvent(QCloseEvent* event);

private:
  Ui::CopyDialog* ui;
  QSharedPointer<Verse> m_currVerse = Verse::current();
  QSharedPointer<DBManager> m_dbMgr = DBManager::current();
  void copyRange();

  QIntValidator* m_verseValidator = new QIntValidator(this);
};

#endif // COPYDIALOG_H
