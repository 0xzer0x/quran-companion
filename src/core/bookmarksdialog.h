#ifndef BOOKMARKSDIALOG_H
#define BOOKMARKSDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include "../utils/dbmanager.h"

namespace Ui {
class BookmarksDialog;
}

class BookmarksDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookmarksDialog(QWidget *parent = nullptr,
                             QString iconPath = ":/assets/images/light/",
                             DBManager *dbMgr = nullptr,
                             int qcfVer = 1);
    void showWindow();
    void loadFavorites();
    ~BookmarksDialog();

signals:
    void navigateToVerse(DBManager::Verse v);

public slots:
    void btnGoToVerse();
    void btnRemove();
    void btnNextClicked();
    void btnPrevClicked();

private:
    Ui::BookmarksDialog *ui;
    DBManager *m_dbMgr = nullptr;
    int m_qcfVer = 1;
    int m_startIdx = 0;
    QString m_iconsPath;
    QString m_fontPrefix;
    QVBoxLayout *m_scrollAreaLayout;
    QList<DBManager::Verse> m_favorites;
    QList<QFrame *> m_frames;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // BOOKMARKSDIALOG_H
