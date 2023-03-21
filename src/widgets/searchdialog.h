#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include "../utils/dbmanager.h"
#include "clickablelabel.h"
typedef DBManager::Verse Verse;

namespace Ui {
class SearchDialog;
}

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(QWidget *parent = nullptr, DBManager *dbPtr = nullptr);
    ~SearchDialog();

signals:
    void navigateToVerse(Verse v);

public slots:
    void getResults();
    void verseClicked();

private:
    Ui::SearchDialog *ui;
    DBManager *m_dbPtr;
    QList<ClickableLabel *> m_lbLst;
    QString m_searchText;

    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
};

#endif // SEARCHDIALOG_H
