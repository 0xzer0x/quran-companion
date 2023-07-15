#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include "../globals.h"
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
                           DBManager* dbMgr = nullptr);
  ~BookmarksDialog();

  void showWindow();
  void loadBookmarks(int surah = -1);
  void loadSurahs();

signals:
  void navigateToVerse(Verse v);

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
  const QDir& m_resources = g_themeResources;
  const QString& m_fontPrefix = g_qcfFontPrefix;
  const int m_qcfVer = g_qcfVersion;
  void setupConnections();
  void addEmptyBookmarksLabel();
  int m_startIdx = 0;
  int m_shownSurah = 0;
  Ui::BookmarksDialog* ui;
  DBManager* m_dbMgr = nullptr;
  QVBoxLayout* m_scrollAreaLayout;
  QList<Verse> m_allBookmarked;
  QList<Verse> m_shownVerses;
  QList<QFrame*> m_frames;
  QStandardItemModel m_surahsModel;
  QStringList m_favSurahList;
};

#endif // BOOKMARKSDIALOG_H
