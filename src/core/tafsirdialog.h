#ifndef TAFSIRDIALOG_H
#define TAFSIRDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include <QDialog>
#include <QSettings>
#include <QShortcut>

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
  explicit TafsirDialog(QWidget* parent = nullptr, DBManager* dbPtr = nullptr);
  ~TafsirDialog();

  void loadVerseTafsir();
  void setShownVerse(const Verse& newShownVerse);

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);
  void showEvent(QShowEvent* event);

private slots:
  void btnNextClicked();
  void btnPrevClicked();

private:
  const int m_qcfVer = Globals::qcfVersion;
  const QDir& m_resources = Globals::themeResources;
  const QString& m_fontPrefix = Globals::qcfFontPrefix;
  const QSettings* m_settings = Globals::settings;
  Ui::TafsirDialog* ui;
  void setupConnections();
  void setTafsirAsTitle();
  int m_fontSZ;
  DBManager* m_dbMgr;
  Verse m_shownVerse{ 1, 1, 1 };
};

#endif // TAFSIRDIALOG_H
