#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include <QStyle>
#include "../utils/dbmanager.h"
#include "../widgets/clickablelabel.h"
#include "../widgets/verseframe.h"

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
    explicit SearchDialog(QWidget *parent = nullptr,
                          int qcfVersion = 1,
                          DBManager *dbPtr = nullptr,
                          const QString &iconPath = ":/assets/images/light/");
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
    int m_qcfVer;
    QString m_iconsPath;
    QString m_fontPrefix;
    DBManager *m_dbPtr;
    QList<VerseFrame *> m_lbLst;
    QString m_searchText;
    QList<Verse> m_currResults;
};

#endif // SEARCHDIALOG_H
