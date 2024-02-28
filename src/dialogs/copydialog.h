#ifndef COPYDIALOG_H
#define COPYDIALOG_H

#include <QClipboard>
#include <QDialog>
#include <QIntValidator>
#include <QMessageBox>
#include <QPointer>
#include <notifiers/copynotifier.h>
#include <types/verse.h>

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
  const CopyNotifier* notifier() const;

public slots:
  /**
   * @brief copy to clipboard the text of the verse with the given index
   * @param IdxInPage - verse index relative to the start of the page
   */
  void copyVerseText(const Verse v);

protected:
  void closeEvent(QCloseEvent* event);

private slots:
  void copyRange();

private:
  Ui::CopyDialog* ui;
  QSharedPointer<Verse> m_currVerse = Verse::current();
  QSharedPointer<QuranDb> m_quranDb = QuranDb::current();
  QPointer<QIntValidator> m_verseValidator;
  CopyNotifier m_notifier;
};

#endif // COPYDIALOG_H
