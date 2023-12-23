#ifndef KHATMAHDIALOG_H
#define KHATMAHDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../widgets/inputfield.h"
#include <QDialog>
#include <QLineEdit>
#include <QSet>

namespace Ui {
class KhatmahDialog;
}

class KhatmahDialog : public QDialog
{
  Q_OBJECT

public:
  explicit KhatmahDialog(const Verse& curr, QWidget* parent = nullptr);
  ~KhatmahDialog();
  void show();

signals:
  void navigateToVerse(Verse v);

protected:
  void closeEvent(QCloseEvent* event);

private slots:
  void startNewKhatmah();
  void renameKhatmah(QString name);
  void removeKhatmah();
  void setActiveKhatmah();

private:
  Ui::KhatmahDialog* ui;
  QSettings* m_settings = Globals::settings;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  const Verse& m_currVerse;

  void loadAll();
  InputField* loadKhatmah(const int id);

  QFrame* m_currActive = nullptr;
  QList<QFrame*> m_frmLst;
  QList<int> m_khatmahId;
  QHash<int, QString> m_names;
};

#endif // KHATMAHDIALOG_H
