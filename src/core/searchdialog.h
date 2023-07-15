#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include "../globals.h"
#include "../utils/dbmanager.h"
#include "../widgets/clickablelabel.h"
#include "../widgets/verseframe.h"
#include <QDialog>
#include <QScrollBar>
#include <QSettings>
#include <QShortcut>
#include <QSpinBox>
#include <QStandardItem>
#include <QStandardItemModel>

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
  explicit SearchDialog(QWidget* parent = nullptr, DBManager* dbPtr = nullptr);
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
  const QDir& m_resources = g_themeResources;
  const QString& m_fontPrefix = g_qcfFontPrefix;
  void setupConnections();
  void fillListView();
  int m_qcfVer;
  int m_startResult = 0;
  Ui::SearchDialog* ui;
  DBManager* m_dbMgr;
  QList<HighlightFrame*> m_lbLst;
  QList<Verse> m_currResults;
  QMap<QString, int> m_selectedSurahMap;
  QString m_searchText;
  QStringList m_surahNames;
  QStandardItemModel m_modelAllSurahs;
  QStandardItemModel m_modelSelectedSurahs;
};

#endif // SEARCHDIALOG_H
