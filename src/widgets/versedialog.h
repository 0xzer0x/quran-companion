#ifndef VERSEDIALOG_H
#define VERSEDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../utils/verse.h"
#include <QDateTime>
#include <QDialog>

namespace Ui {
class VerseDialog;
}

class VerseDialog : public QDialog
{
  Q_OBJECT

public:
  explicit VerseDialog(QWidget* parent = nullptr);
  ~VerseDialog();
  /**
   * @brief checks if the verse of the day has been set and set it if not found
   */
  bool votdShown();

public slots:
  void showVOTD(bool startup);

signals:
  void navigateToVerse(Verse v);

protected:
  void mouseReleaseEvent(QMouseEvent* event);
  void closeEvent(QCloseEvent* event);

private:
  Ui::VerseDialog* ui;
  const QSettings* m_settings = Globals::settings;
  DBManager* m_dbMgr = qobject_cast<DBManager*>(Globals::databaseManager);
  QFile m_timestampFile = Globals::configDir.absoluteFilePath("votd.log");
  fa::QtAwesome* m_fa = Globals::awesome;
  /**
   * @brief generate the verse of the day and set the votd html
   */
  void genVerseOfTheDay();
  /**
   * @brief read the verse of the day from the timestamp file
   */
  void readVerseOfTheDay();
  /**
   * @brief write the current time and votd to the timestamp file
   */
  void writeTimestamp();
  /**
   * @brief serialize the verse of the day into a QString
   * @return QString of PAGE:SURA:NUMBER
   */
  QString votdStringEntry() const;

  void updateLabels();
  /**
   * @brief m_votd
   */
  Verse m_votd;
  QDateTime m_now = QDateTime::currentDateTime();
};

#endif // VERSEDIALOG_H
