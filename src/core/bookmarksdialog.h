#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include "../utils/dbmanager.h"
#include <QDialog>
#include <QLabel>
#include <QShortcut>
#include <QStandardItem>
#include <QStringListModel>
#include <QVBoxLayout>
#include <set>

namespace Ui {
class BookmarksDialog;
}

class BookmarksDialog : public QDialog
{
  Q_OBJECT

public:
  explicit BookmarksDialog(QWidget* parent = nullptr,
                           QString iconPath = ":/resources/light/",
                           DBManager* dbMgr = nullptr,
                           int qcfVer = 1);
  void showWindow();
  void loadBookmarks(int surah = -1);
  void loadSurahs();
  ~BookmarksDialog();

signals:
  void navigateToVerse(DBManager::Verse v);

public slots:
  void btnGoToVerse();
  void btnRemove();
  void btnNextClicked();
  void btnPrevClicked();

  // QWidget interface
protected:
  void closeEvent(QCloseEvent* event);

private slots:
  void surahSelected(const QModelIndex& index);

private:
  void setupConnections();
  Ui::BookmarksDialog* ui;
  DBManager* m_dbMgr = nullptr;
  int m_qcfVer = 1;
  int m_startIdx = 0;
  int m_shownSurah = 0;
  QString m_resourcePath;
  QString m_fontPrefix;
  QVBoxLayout* m_scrollAreaLayout;
  QList<DBManager::Verse> m_allBookmarked;
  QList<DBManager::Verse> m_shownVerses;
  QList<QFrame*> m_frames;
  QStandardItemModel m_surahsModel;
  QStringList m_favSurahList;
};

#endif // BOOKMARKSDIALOG_H
