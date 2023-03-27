#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QStyle>
#include "../utils/dbmanager.h"
#include "clickablelabel.h"
#include "verseframe.h"

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
    explicit SearchDialog(QWidget *parent = nullptr, DBManager *dbPtr = nullptr);
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
    void closeEvent(QCloseEvent *event);

private:
    Ui::SearchDialog *ui;
    int m_startResult = 0;
    DBManager *m_dbPtr;
    QList<VerseFrame *> m_lbLst;
    QString m_searchText;
    QList<Verse> m_currResults;
};

#endif // SEARCHDIALOG_H
