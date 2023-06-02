#ifndef TAFSIRDIALOG_H
#define TAFSIRDIALOG_H

#include "../utils/dbmanager.h"
#include <QDialog>
#include <QSettings>
typedef DBManager::Verse Verse;

namespace Ui {
class TafsirDialog;
}

/*!
 * \class TafsirDialog
 * \brief The TafsirDialog class displays tafsir for a specific verse
 *
 * Tafsir is shown for a single verse at a time. Navigation between verses is
 * independant of the main Quran reader navigation for easier navigation.
 */
class TafsirDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TafsirDialog(QWidget* parent = nullptr,
                        DBManager* dbPtr = nullptr,
                        QSettings* settings = nullptr,
                        const QString& iconsPath = ":/images/light/");
  ~TafsirDialog();

  void loadVerseTafsir();
  void setShownVerse(const Verse& newShownVerse);

private slots:
  void btnNextClicked();
  void btnPrevClicked();

private:
  Ui::TafsirDialog* ui;
  void setupConnections();
  void setTafsirAsTitle();
  int m_fontSZ;
  QString m_fontPrefix;
  QSettings* m_settings;
  DBManager* m_dbMgr;
  Verse m_shownVerse{ 1, 1, 1 };

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);
  void showEvent(QShowEvent* event);
};

#endif // TAFSIRDIALOG_H
