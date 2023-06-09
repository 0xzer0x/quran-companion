#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "../utils/dbmanager.h"
#include "../widgets/clickablelabel.h"
#include "../widgets/verseframe.h"
#include <QDialog>
#include <QScrollBar>
#include <QSettings>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>

typedef DBManager::Verse Verse;

namespace Ui {
class SearchDialog;
}

/*!
 * \class SearchDialog
 *
 * \brief The SearchDialog class is responsible for searching quran verses
 */
class SearchDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SearchDialog(QWidget* parent = nullptr,
                        QSettings* settings = nullptr,
                        DBManager* dbPtr = nullptr,
                        const QString& iconPath = ":/resources/light/");
  ~SearchDialog();

public slots:
  void getResults();
  void verseClicked();
  void showResults();
  void moveFwd();
  void moveBwd();

signals:
  void navigateToVerse(Verse v);

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);

private slots:
  void btnTransferClicked();

private:
  void setupConnections();
  void fillListView();
  int m_qcfVer;
  int m_startResult = 0;
  Ui::SearchDialog* ui;
  DBManager* m_dbMgr;
  QSettings* m_settings;
  QString m_resourcePath;
  QString m_fontPrefix;
  QStringList m_surahNames;
  QMap<QString, int> m_selectedSurahMap;
  QList<HighlightFrame*> m_lbLst;
  QString m_searchText;
  QList<Verse> m_currResults;
  QStandardItemModel m_modelAllSurahs;
  QStandardItemModel m_modelSelectedSurahs;
};

#endif // SEARCHDIALOG_H
