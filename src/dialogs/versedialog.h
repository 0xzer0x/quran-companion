#ifndef VERSEDIALOG_H
#define VERSEDIALOG_H

#include <QDateTime>
#include <QDialog>
#include <database/translationdb.h>
#include <types/verse.h>
#include <utils/dirmanager.h>

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
  void navigateToVerse(const Verse& v);

protected:
  void mouseReleaseEvent(QMouseEvent* event);
  void closeEvent(QCloseEvent* event);

private:
  Ui::VerseDialog* ui;
  Configuration& m_config;
  const QuranDb& m_quranDb;
  const TranslationDb& m_translationDb;
  QFile m_timestampFile;
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
