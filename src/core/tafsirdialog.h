#ifndef TAFSIRDIALOG_H
#define TAFSIRDIALOG_H

#include "../utils/dbmanager.h"
#include <QDialog>
#include <QSettings>
typedef DBManager::Verse Verse;

namespace Ui {
class TafsirDialog;
}

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
  void setTafsirAsTitle();
  int m_fontSZ;
  QString m_fontPrefix;
  Ui::TafsirDialog* ui;
  QSettings* m_settings;
  DBManager* m_dbMgr;
  Verse m_shownVerse{ 1, 1, 1 };

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);
  void showEvent(QShowEvent* event);
};

#endif // TAFSIRDIALOG_H
